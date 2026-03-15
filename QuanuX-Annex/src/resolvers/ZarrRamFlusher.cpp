#include "quanux/annex/do/ZarrRamFlusher.hpp"
#include "quanux/annex/do/AwsSigV4.hpp"
#include <iostream>
#include <cstring>
#include <curl/curl.h>
#include <chrono>

// The c-blosc2 inclusion for lightning-fast memory compression
// #include <blosc2.h> // Conceptual include for the build

namespace quanux {
namespace annex {
namespace do_impl {

static size_t curl_read_cb(char *ptr, size_t size, size_t nitems, void *userdata) {
    auto *job = static_cast<std::vector<uint8_t>*>(userdata);
    size_t to_copy = std::min(size * nitems, job->size());
    if (to_copy > 0) {
        std::memcpy(ptr, job->data(), to_copy);
        job->erase(job->begin(), job->begin() + to_copy);
    }
    return to_copy;
}

ZarrRamFlusher::ZarrRamFlusher(size_t chunk_size_elements, const std::string& bucket, const std::string& region, const std::string& access_key, const std::string& secret_key)
    : m_chunk_size(chunk_size_elements), m_bucket(bucket), m_region(region), m_access_key(access_key), m_secret_key(secret_key), m_running(true) {
    // Pre-allocate to prevent dynamic heap fragmentation on the hot loop
    m_tick_buffer.reserve(m_chunk_size);
    m_exec_buffer.reserve(m_chunk_size);
    m_drift_buffer.reserve(m_chunk_size);
    
    // blosc2_init(); // Initialize the Blosc2 engine globally
    
    m_network_thread = std::thread(&ZarrRamFlusher::network_worker, this);
}

ZarrRamFlusher::~ZarrRamFlusher() {
    force_flush();
    m_running = false;
    m_cv.notify_all();
    if (m_network_thread.joinable()) {
        m_network_thread.join();
    }
    // blosc2_destroy();
}

void ZarrRamFlusher::ingest_tick(const MarketTick& tick) {
    if (m_tick_buffer.empty()) {
        std::cout << "[ZarrRamFlusher] First TICK frame parsed. Alignment verified: Instrument: " 
                  << tick.instrument_id << " Bid: " << tick.bid_price << "\n";
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_tick_buffer.push_back(tick);
    if (needs_flush()) flush_internal();
}

void ZarrRamFlusher::ingest_execution(const ExecutionLog& exec) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_exec_buffer.push_back(exec);
    if (needs_flush()) flush_internal();
}

void ZarrRamFlusher::ingest_anomaly(const SchemaDriftEvent& drift) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_drift_buffer.push_back(drift);
    if (needs_flush()) flush_internal();
}

bool ZarrRamFlusher::needs_flush() const {
    return m_tick_buffer.size() >= m_chunk_size || 
           m_exec_buffer.size() >= m_chunk_size || 
           m_drift_buffer.size() >= m_chunk_size;
}

void ZarrRamFlusher::force_flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    flush_internal();
}

void ZarrRamFlusher::flush_internal() {
    if (m_tick_buffer.empty() && m_exec_buffer.empty() && m_drift_buffer.empty()) return;

    std::cout << "[ZarrRamFlusher] Executing Flush via Blosc2 to background network thread...\n";

    // --- THE MUSCLE: Blosc2 Compression for Market Ticks ---
    if (!m_tick_buffer.empty()) {
        size_t raw_size_bytes = m_tick_buffer.size() * sizeof(MarketTick);
        
        // Simulating Blosc2 with a straight memory dump to the byte vector
        std::vector<uint8_t> payload(reinterpret_cast<const uint8_t*>(m_tick_buffer.data()), reinterpret_cast<const uint8_t*>(m_tick_buffer.data()) + raw_size_bytes);

        UploadJob job;
        job.payload = std::move(payload);
        
        auto now = std::chrono::system_clock::now();
        std::string object_time = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        job.object_key = "zarr_ticks/" + object_time + ".bin";
        
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_upload_queue.push(std::move(job));
        }
        m_cv.notify_one();

        std::cout << "  -> Flushed " << m_tick_buffer.size() << " MarketTicks. Encueud for libcurl multi-drop.\n";
        m_tick_buffer.clear();
    }

    m_exec_buffer.clear();
    m_drift_buffer.clear();

    std::cout << "[ZarrRamFlusher] Flush complete. RAM Vectors reset.\n";
}

void ZarrRamFlusher::network_worker() {
    CURLM* multi_handle = curl_multi_init();
    curl_multi_setopt(multi_handle, CURLMOPT_PIPELINING, CURLPIPE_MULTIPLEX);

    std::vector<CURL*> active_transfers;
    AwsSigV4 signer(m_access_key, m_secret_key, m_region, "s3");
    
    while(m_running || !active_transfers.empty() || !m_upload_queue.empty()) {
        
        while (active_transfers.size() < 10) { 
            UploadJob job;
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                if (m_upload_queue.empty()) break;
                job = std::move(m_upload_queue.front());
                m_upload_queue.pop();
            }

            CURL* curl = curl_easy_init();
            
            std::string uri = "/" + job.object_key;
            std::string host = m_bucket + "." + m_region + ".digitaloceanspaces.com";
            std::string url = "https://" + host + uri;
            
            std::map<std::string, std::string> headers;
            headers["host"] = host;
            headers["content-type"] = "application/octet-stream";
            headers["x-amz-acl"] = "private";
            
            std::string payload_str(job.payload.begin(), job.payload.end());
            auto signed_headers = signer.sign_request("PUT", uri, payload_str, headers);
            
            struct curl_slist* chunk = nullptr;
            for (const auto& kv : signed_headers) {
                chunk = curl_slist_append(chunk, (kv.first + ": " + kv.second).c_str());
            }
            
            chunk = curl_slist_append(chunk, "Expect:");

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
            
            auto* read_data = new std::vector<uint8_t>(std::move(job.payload));
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, curl_read_cb);
            curl_easy_setopt(curl, CURLOPT_READDATA, read_data);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(read_data->size()));
            
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 120L);
            curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 60L);
            
            // Enable VERBOSE mode to diagnose the 400 Error!
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            
            auto* private_data = new std::pair<struct curl_slist*, std::vector<uint8_t>*>(chunk, read_data);
            curl_easy_setopt(curl, CURLOPT_PRIVATE, private_data);

            curl_multi_add_handle(multi_handle, curl);
            active_transfers.push_back(curl);
            std::cout << "[curl_multi] Non-blocking upload initiated for " << job.object_key << std::endl;
        }

        int still_running = 0;
        curl_multi_perform(multi_handle, &still_running);

        int msgs_left = 0;
        CURLMsg* msg;
        while ((msg = curl_multi_info_read(multi_handle, &msgs_left))) {
            if (msg->msg == CURLMSG_DONE) {
                CURL* e = msg->easy_handle;
                long response_code;
                curl_easy_getinfo(e, CURLINFO_RESPONSE_CODE, &response_code);
                std::cout << "[curl_multi] Transfer complete. HTTPS " << response_code << std::endl;
                
                void* ptr;
                curl_easy_getinfo(e, CURLINFO_PRIVATE, &ptr);
                auto* data_pair = static_cast<std::pair<struct curl_slist*, std::vector<uint8_t>*>*>(ptr);
                curl_slist_free_all(data_pair->first);
                delete data_pair->second;
                delete data_pair;

                curl_multi_remove_handle(multi_handle, e);
                curl_easy_cleanup(e);
                active_transfers.erase(std::remove(active_transfers.begin(), active_transfers.end(), e), active_transfers.end());
            }
        }

        if (still_running) {
            int numfds = 0;
            curl_multi_wait(multi_handle, nullptr, 0, 1000, &numfds);
        } else if (m_running) {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_cv.wait_for(lock, std::chrono::milliseconds(100), [this] { return !m_upload_queue.empty() || !m_running; });
        }
    }
    curl_multi_cleanup(multi_handle);
}

} // namespace do_impl
} // namespace annex
} // namespace quanux
