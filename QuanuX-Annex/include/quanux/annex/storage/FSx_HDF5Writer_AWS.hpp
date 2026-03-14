#pragma once

#include "quanux/annex/TelemetryExhaust.hpp"
#include <vector>
#include <string>

namespace quanux {
namespace annex {
namespace storage {

/**
 * @brief QuanuX-Annex AWS Write-Path Daemon (FSx POSIX Writer)
 * 
 * Taps Redpanda directly and writes mem-aligned structs into HDF5 datasets
 * via the native C-API onto a local Lustre mount-point.
 */
class FSx_HDF5Writer_AWS {
public:
    explicit FSx_HDF5Writer_AWS(const std::string& mount_path);
    ~FSx_HDF5Writer_AWS();

    void ingest_tick(const MarketTick& tick);

private:
    std::string m_mount_path;
    size_t m_chunk_size_elements;
    std::vector<MarketTick> m_tick_buffer;

    void execute_lustre_flush();
};

} // namespace storage
} // namespace annex
} // namespace quanux
