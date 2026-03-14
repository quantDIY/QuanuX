#pragma once

#include "quanux/annex/TelemetryExhaust.hpp"
#include <vector>
#include <string>

namespace quanux {
namespace annex {
namespace storage {

/**
 * @brief QuanuX-Annex GCP Write-Path Daemon (HSDS Shredder)
 * 
 * Explicitly traps unknown FIX 99999 anomalies (SchemaDrift) and packages
 * them into an unstructured JSONB column, protecting the strict BigQuery GraphQL SQL transpiler.
 */
class BigQuery_HDF5Shredder_GCP {
public:
    BigQuery_HDF5Shredder_GCP();
    ~BigQuery_HDF5Shredder_GCP();

    void route_market_tick(const MarketTick& tick);
    void route_schema_drift(const SchemaDriftEvent& drift);

    void shred_to_storage();

private:
    std::vector<MarketTick> m_structured_ticks;
    std::vector<std::string> m_jsonb_drift_column; // The Generic Unstructured JSON Bucket
};

} // namespace storage
} // namespace annex
} // namespace quanux
