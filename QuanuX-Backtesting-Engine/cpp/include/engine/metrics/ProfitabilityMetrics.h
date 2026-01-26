#pragma once
#include "MetricBase.h"
#include <numeric>

namespace quanux::engine::metrics {

class ProfitabilityMetrics {
public:
  struct Result {
    double total_net_profit = 0.0;
    double gross_profit = 0.0;
    double gross_loss = 0.0;
    double profit_factor = 0.0;
    double win_rate = 0.0;
    double average_trade = 0.0; // Expectancy
    int total_trades = 0;
    int winning_trades = 0;
    int losing_trades = 0;
  };

  static Result calculate(const std::vector<TradeRecord> &trades) {
    Result r;
    r.total_trades = trades.size();
    if (r.total_trades == 0)
      return r;

    for (const auto &t : trades) {
      double pnl = t.net_profit();
      r.total_net_profit += pnl;

      if (pnl > 0) {
        r.gross_profit += pnl;
        r.winning_trades++;
      } else {
        r.gross_loss += std::abs(pnl); // Absolute value for loss
        r.losing_trades++;
      }
    }

    r.profit_factor = (r.gross_loss == 0)
                          ? std::numeric_limits<double>::infinity()
                          : (r.gross_profit / r.gross_loss);

    r.win_rate = static_cast<double>(r.winning_trades) / r.total_trades;
    r.average_trade = r.total_net_profit / r.total_trades;

    return r;
  }
};

} // namespace quanux::engine::metrics
