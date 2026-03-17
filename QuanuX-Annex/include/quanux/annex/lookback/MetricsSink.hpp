#pragma once
#include <string>

namespace quanux {
namespace annex {
namespace lookback {

class MetricsSink {
public:
    // Ingests real-time metrics pushed down from Aleph's OpenSearch and Vector engines
    // Enqueues them for permanent storage in the Annex HDF5 Sovereign Vault
    static void sinkMetric(const std::string& metricJson);
};

} // namespace lookback
} // namespace annex
} // namespace quanux
