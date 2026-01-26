#pragma once
#include "MetricBase.h"
#include "ProfitabilityMetrics.h"
#include "RiskMetrics.h"

namespace quanux::engine::metrics {

// Facade for easy access
struct BacktestReport {
  ProfitabilityMetrics::Result profitability;
  RiskMetrics::Result risk;
};

// ... Helper to generate full report from raw data

} // namespace quanux::engine::metrics
