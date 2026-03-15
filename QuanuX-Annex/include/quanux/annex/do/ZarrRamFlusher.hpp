#pragma once

#include <vector>
#include <mutex>
#include <cstdint>
#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include "quanux/annex/TelemetryExhaust.hpp"

namespace quanux {
namespace annex {
namespace do_impl {

/**
 * @brief In-memory columnar Zarr buffer for pre-computation.
 * Caches ticks, executions, and anomalies before flushing chunks to DO Spaces.
 */
class ZarrRamFlusher {
public:
    ZarrRamFlusher(size_t chunk_size_elements, const std::string& bucket, const std::string& region, const std::string& access_key, const std::string& secret_key);
    ~ZarrRamFlusher();

    // Ingest events into columnar RAM arrays
    void ingest_tick(const MarketTick& tick);
    void ingest_execution(const ExecutionLog& exec);
    void ingest_anomaly(const SchemaDriftEvent& drift);

    // Forces a flush of the current buffer to the Virtual Filesystem
    void force_flush();

    // Check if bounds have triggered a natural flush
    bool needs_flush() const;

private:
    size_t m_chunk_size;
    std::string m_bucket;
    std::string m_region;
    std::string m_access_key;
    std::string m_secret_key;

    std::mutex m_mutex;

    // Pre-allocated contiguous vectors to avoid reallocation during ingestion
    std::vector<MarketTick> m_tick_buffer;
    std::vector<ExecutionLog> m_exec_buffer;
    std::vector<SchemaDriftEvent> m_drift_buffer;

    // Curl Multi Async Thread Mechanics
    struct UploadJob {
        std::vector<uint8_t> payload;
        std::string object_key;
    };

    std::queue<UploadJob> m_upload_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running;
    std::thread m_network_thread;

    void flush_internal();
    void network_worker();
};

} // namespace do_impl
} // namespace annex
} // namespace quanux
