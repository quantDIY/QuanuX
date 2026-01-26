#pragma once
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace quanux::engine::metrics {

/**
 * @brief Base class for all performance metrics.
 * Designed to be stateless calculators or stateful accumulators depending on
 * need. For now, we assume post-processing calculation on equity curves or
 * trade lists.
 */
class MetricBase {
public:
  virtual ~MetricBase() = default;

  virtual std::string name() const = 0;
  virtual std::string description() const = 0;
};

struct TradeRecord {
  double entry_price;
  double exit_price;
  double qty;
  bool is_long;
  uint64_t entry_time;
  uint64_t exit_time;
  double commission = 0.0;

  double net_profit() const {
    double raw_pl = (exit_price - entry_price) * qty;
    if (!is_long)
      raw_pl = -raw_pl;
    return raw_pl - commission;
  }
};

} // namespace quanux::engine::metrics
