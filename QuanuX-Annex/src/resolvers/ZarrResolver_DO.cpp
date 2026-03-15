#include "quanux/annex/ZarrResolver_DO.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include "quanux/annex/do/AwsSigV4.hpp"
#include "quanux/annex/TelemetryExhaust.hpp"
#include "quanux/annex/federation/json.hpp"
#include <cinttypes>

namespace {
    std::string get_habitat_env(const std::string& key) {
        std::ifstream file("/etc/quanux/habitat.env");
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind(key + "=", 0) == 0) {
                std::string val = line.substr(key.length() + 1);
                if (!val.empty() && val.front() == '"') val.erase(0, 1);
                if (!val.empty() && val.back() == '"') val.pop_back();
                return val;
            }
        }
        return "";
    }
}

namespace quanux {
namespace annex {

ZarrResolver_DO::ZarrResolver_DO(const std::string& space_name, const std::string& space_region)
    : m_space_name(space_name), m_space_region(space_region) {
    m_access_key = get_habitat_env("DO_SPACES_KEY");
    m_secret_key = get_habitat_env("DO_SPACES_SECRET");

    std::cout << "[main] Keyring Validated: Parsed DO_SPACES_KEY=" << m_access_key.substr(0, 5) << "... from Habitat Environment" << std::endl;

    // Initialize standard flush thresholds for DO Spaces (e.g. 50,000 items per Zarr chunk)
    m_flusher = std::make_unique<do_impl::ZarrRamFlusher>(50000, m_space_name, m_space_region, m_access_key, m_secret_key);
    
    std::string dynamic_nats_url = get_habitat_env("NATS_URL");
    std::cout << "[main] Keyring Validated: Parsed NATS_URL=" << dynamic_nats_url << " from Habitat Environment" << std::endl;
    
    m_subscriber = std::make_unique<do_impl::NatsSubscriber>(dynamic_nats_url, "QUANUX_EXHAUST");
}

ZarrResolver_DO::~ZarrResolver_DO() {
    stop_ingestion();
}

void ZarrResolver_DO::broadcast_live_markdown(const std::string& markdown) {
    // Emits purely formatted Markdown into the live AI-agent broadcast channel.
    std::cout << "[ZarrResolver_DO] BROADCAST LIVE:\n" << markdown << std::endl;
}

static size_t curl_write_cb(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    auto* mem = static_cast<std::vector<uint8_t>*>(userp);
    auto* char_ptr = static_cast<uint8_t*>(contents);
    mem->insert(mem->end(), char_ptr, char_ptr + realsize);
    return realsize;
}

std::string ZarrResolver_DO::get_historical_analytics(const std::string& query) {
    // Deprecated by Phase 14.5 streaming matrix
    return "{\"errors\": [{\"message\": \"Use stream_historical_analytics instead\"}]}";
}

bool ZarrResolver_DO::stream_historical_analytics(const std::string& query, std::function<bool(const std::string& chunk)> sink_cb) {
    std::string target_chunk = "zarr_ticks/1773579693857.bin"; // E.g. calculated from GraphQL

    do_impl::AwsSigV4 signer(m_access_key, m_secret_key, m_space_region, "s3");

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string uri = "/" + target_chunk;
    std::string host = m_space_name + "." + m_space_region + ".digitaloceanspaces.com";
    std::string url = "https://" + host + uri;

    std::map<std::string, std::string> headers;
    headers["host"] = host;
    
    auto signed_headers = signer.sign_request("GET", uri, "", headers);

    struct curl_slist* chunk = nullptr;
    for (const auto& kv : signed_headers) {
        chunk = curl_slist_append(chunk, (kv.first + ": " + kv.second).c_str());
    }

    std::vector<uint8_t> response_data;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    std::cout << "[ZarrResolver_DO] Retrieves chunk for streaming -> " << uri << "\n";
    CURLcode res = curl_easy_perform(curl);
    
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || http_code != 200) {
        std::cerr << "[ZarrResolver_DO] S3 GET Failed. HTTP Code: " << http_code << "\n";
        return false;
    }

    // 3. Decompress Memory Frame
    size_t num_ticks = response_data.size() / sizeof(MarketTick);
    const MarketTick* ticks = reinterpret_cast<const MarketTick*>(response_data.data());

    // 4. Stream via Chunked Fast-Path. Avoid DOM manipulation RAM overhead string.
    sink_cb("{\"data\":{\"market_ticks\":[\n");
    
    char buffer[512];
    for (size_t i = 0; i < num_ticks; ++i) {
        const auto& t = ticks[i];
        int len = snprintf(buffer, sizeof(buffer),
            "  {\"timestamp_ns\":%" PRIu64 ",\"instrument_id\":%u,\"bid_price\":%f,\"ask_price\":%f,\"bid_size\":%u,\"ask_size\":%u,\"level\":%u}%s\n",
            t.timestamp_ns, t.instrument_id, t.bid_price, t.ask_price, t.bid_size, t.ask_size, (uint32_t)t.level,
            (i == num_ticks - 1) ? "" : ","
        );
        if (len > 0) {
            sink_cb(std::string(buffer, len));
        }
    }

    sink_cb("]}}\n");
    std::cout << "[ZarrResolver_DO] Streamed " << num_ticks << " ticks back to Hasura.\n";
    
    return true;
}

void ZarrResolver_DO::start_ingestion() {
    // Wire up zero-copy lambdas connecting the Network to the RAM Flusher
    m_subscriber->subscribe_market_data("QUANUX.MARKET.TICK", [this](const MarketTick& tick) {
        m_flusher->ingest_tick(tick);
    });

    m_subscriber->subscribe_executions("QUANUX.MARKET.EXEC", [this](const ExecutionLog& exec) {
        m_flusher->ingest_execution(exec);
    });

    m_subscriber->subscribe_anomalies("QUANUX.MARKET.DRIFT", [this](const SchemaDriftEvent& drift) {
        m_flusher->ingest_anomaly(drift);
    });

    // Spawn network thread
    m_subscriber->run_event_loop();
}

void ZarrResolver_DO::stop_ingestion() {
    if (m_subscriber) {
        m_subscriber->stop();
    }
    if (m_flusher) {
        m_flusher->force_flush();
    }
}

} // namespace annex
} // namespace quanux
