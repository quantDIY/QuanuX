#pragma once
#include "MetricBase.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace quanux::engine::metrics {

class RiskMetrics {
public:
  struct Result {
    double max_drawdown_pct = 0.0;
    double max_drawdown_abs = 0.0;
    double sharpe_ratio = 0.0;
    double sortino_ratio = 0.0;
    double calmar_ratio = 0.0;
    double annualized_volatility = 0.0;
    double cagr = 0.0;
  };

  // Assuming periodic returns (e.g., daily) for Sharpe/Sortino
  // and equity curve for Drawdown
  static Result calculate(const std::vector<double> &equity_curve,
                          const std::vector<double> &returns,
                          double risk_free_rate = 0.0,
                          int periods_per_year = 252) {
    Result r;
    if (equity_curve.empty() || returns.empty())
      return r;

    // 1. Drawdown Analysis
    double peak = -std::numeric_limits<double>::infinity();
    double max_dd_abs = 0.0;
    double max_dd_pct = 0.0;

    for (double equity : equity_curve) {
      if (equity > peak) {
        peak = equity;
      }
      double dd_abs = peak - equity;
      double dd_pct = (peak > 0) ? (dd_abs / peak) : 0.0;

      max_dd_abs = std::max(max_dd_abs, dd_abs);
      max_dd_pct = std::max(max_dd_pct, dd_pct);
    }
    r.max_drawdown_abs = max_dd_abs;
    r.max_drawdown_pct = max_dd_pct;

    // 2. Returns Statistics
    double sum_returns = std::accumulate(returns.begin(), returns.end(), 0.0);
    double mean_return = sum_returns / returns.size();

    double sq_sum = std::inner_product(returns.begin(), returns.end(),
                                       returns.begin(), 0.0);
    double stdev =
        std::sqrt(sq_sum / returns.size() - mean_return * mean_return);

    // Annualize
    r.annualized_volatility = stdev * std::sqrt(periods_per_year);
    double annualized_return =
        mean_return * periods_per_year; // Simple approximation

    // 3. Ratios
    if (r.annualized_volatility != 0) {
      r.sharpe_ratio =
          (annualized_return - risk_free_rate) / r.annualized_volatility;
    }

    // Sortino (Downside Deviation)
    double downside_sq_sum = 0.0;
    int downside_count = 0;
    for (double ret : returns) {
      if (ret < 0) { // Or less than required rate? Typically < 0 for simplicity
        downside_sq_sum += ret * ret;
        downside_count++;
      }
    }
    double downside_dev = std::sqrt(downside_sq_sum / returns.size()) *
                          std::sqrt(periods_per_year);

    if (downside_dev != 0) {
      r.sortino_ratio = (annualized_return - risk_free_rate) / downside_dev;
    }

    // Calmar
    if (r.max_drawdown_pct != 0) {
      r.calmar_ratio =
          annualized_return / r.max_drawdown_pct; // Often uses CAGR over 3
                                                  // years, simple approx here
    }

    // CAGR
    if (!equity_curve.empty()) {
      double start_equity = equity_curve.front();
      double end_equity = equity_curve.back();
      double years =
          static_cast<double>(equity_curve.size()) / periods_per_year;
      if (years > 0 && start_equity > 0) {
        r.cagr = std::pow(end_equity / start_equity, 1.0 / years) - 1.0;
      }
    }

    return r;
  }
};

} // namespace quanux::engine::metrics
