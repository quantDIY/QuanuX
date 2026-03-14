#pragma once

#include <vector>
#include <mutex>
#include <cstdint>
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
    explicit ZarrRamFlusher(size_t chunk_size_elements);
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
    std::mutex m_mutex;

    // Pre-allocated contiguous vectors to avoid reallocation during ingestion
    std::vector<MarketTick> m_tick_buffer;
    std::vector<ExecutionLog> m_exec_buffer;
    std::vector<SchemaDriftEvent> m_drift_buffer;

    void flush_internal();
};

} // namespace do_impl
} // namespace annex
} // namespace quanux
