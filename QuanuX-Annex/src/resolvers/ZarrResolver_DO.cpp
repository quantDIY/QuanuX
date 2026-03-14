#include "quanux/annex/ZarrResolver_DO.hpp"
#include <iostream>

namespace quanux {
namespace annex {

ZarrResolver_DO::ZarrResolver_DO(const std::string& space_name, const std::string& space_region)
    : m_space_name(space_name), m_space_region(space_region) {
    // Initialize standard flush thresholds for DO Spaces (e.g. 50,000 items per Zarr chunk)
    m_flusher = std::make_unique<do_impl::ZarrRamFlusher>(50000);
    m_subscriber = std::make_unique<do_impl::NatsSubscriber>("nats://10.0.0.5:4222", "QUANUX_EXHAUST");
}

ZarrResolver_DO::~ZarrResolver_DO() {
    stop_ingestion();
}

void ZarrResolver_DO::broadcast_live_markdown(const std::string& markdown) {
    // Emits purely formatted Markdown into the live AI-agent broadcast channel.
    std::cout << "[ZarrResolver_DO] BROADCAST LIVE:\n" << markdown << std::endl;
}

std::string ZarrResolver_DO::get_historical_analytics(const std::string& query) {
    // Parses query, loads Zarr chunks via S3 protocol out of DO Spaces,
    // and synthesizes the analytics into a Markdown response string.
    return "### Historical Zarr Query Result\n\n* Backend: DigitalOcean Spaces (" + m_space_region + ")\n* Query: `" + query + "`\n\n**Data:** (Simulated response from Zarr blocks)";
}

void ZarrResolver_DO::start_ingestion() {
    // Wire up zero-copy lambdas connecting the Network to the RAM Flusher
    m_subscriber->subscribe_market_data("exhaust.tick.>", [this](const MarketTick& tick) {
        m_flusher->ingest_tick(tick);
    });

    m_subscriber->subscribe_executions("exhaust.exec.>", [this](const ExecutionLog& exec) {
        m_flusher->ingest_execution(exec);
    });

    m_subscriber->subscribe_anomalies("exhaust.anomaly.>", [this](const SchemaDriftEvent& drift) {
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
