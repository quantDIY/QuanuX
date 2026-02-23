#pragma once

#include "engine/metrics/PerformanceAnalyzer.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

namespace quanux::engine::metrics {

struct AdvancedStatsResult {
  double win_rate;
  double avg_win;
  double avg_loss;
  double kelly_fraction;

  // Monte Carlo
  int monte_carlo_iterations;
  double mc_expected_pnl;
  double mc_95_percentile_low_pnl;

  double max_drawdown_dollars;
  double max_drawdown_percent;

  double uncertainty_adjusted_kelly;
};

class AdvancedStatsAnalyzer {
public:
  static AdvancedStatsResult compute(const std::vector<CrucibleTrade> &trades,
                                     int mc_iterations = 1000) {
    AdvancedStatsResult res{};
    res.monte_carlo_iterations = mc_iterations;

    if (trades.empty())
      return res;

    // 1. Core Empirical Distribution
    int wins = 0;
    double gross_profit = 0.0;
    int losses = 0;
    double gross_loss = 0.0;

    double peak_equity = 0.0;
    double max_dd_dollars = 0.0;
    double running_pnl = 0.0;

    std::vector<double> pnl_array;
    pnl_array.reserve(trades.size());

    for (const auto &t : trades) {
      pnl_array.push_back(t.profit);
      running_pnl += t.profit;

      if (running_pnl > peak_equity) {
        peak_equity = running_pnl;
      }

      double dd = peak_equity - running_pnl;
      if (dd > max_dd_dollars) {
        max_dd_dollars = dd;
      }

      if (t.profit > 0) {
        wins++;
        gross_profit += t.profit;
      } else {
        losses++;
        gross_loss += std::abs(t.profit); // absolute value for loss math
      }
    }

    res.max_drawdown_dollars = max_dd_dollars;
    res.max_drawdown_percent =
        (peak_equity > 0) ? (max_dd_dollars / peak_equity) * 100.0 : 0.0;

    res.win_rate = static_cast<double>(wins) / trades.size();
    res.avg_win = (wins > 0) ? (gross_profit / wins) : 0.0;
    res.avg_loss = (losses > 0) ? (gross_loss / losses) : 1.0; // avoid div/zero

    // 2. Standard Kelly Criterion
    // Kelly % = W - [(1 - W) / R]
    // W = Win Probability, R = Win/Loss Ratio
    double R = (res.avg_loss > 0) ? (res.avg_win / res.avg_loss) : 0.0;
    if (R > 0) {
      res.kelly_fraction = res.win_rate - ((1.0 - res.win_rate) / R);
    } else {
      res.kelly_fraction = 0.0;
    }

    // Bound Kelly (No borrowing, no shorting the Kelly)
    if (res.kelly_fraction < 0)
      res.kelly_fraction = 0.0;
    if (res.kelly_fraction > 1.0)
      res.kelly_fraction = 1.0;

    // 3. Monte Carlo Resampling Simulation
    // We resample the PnL array with replacement `mc_iterations` times to build
    // a distribution of Expected Terminal PnL
    std::vector<double> terminal_pnls;
    terminal_pnls.reserve(mc_iterations);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, trades.size() - 1);

    double total_mc_pnl = 0.0;
    for (int i = 0; i < mc_iterations; ++i) {
      double simulated_pnl = 0.0;
      for (size_t t = 0; t < trades.size(); ++t) {
        simulated_pnl += pnl_array[dis(gen)];
      }
      terminal_pnls.push_back(simulated_pnl);
      total_mc_pnl += simulated_pnl;
    }

    res.mc_expected_pnl = total_mc_pnl / mc_iterations;

    // Sort to find 95% Confidence Low (5th Percentile)
    std::sort(terminal_pnls.begin(), terminal_pnls.end());
    int p05_index = static_cast<int>(mc_iterations * 0.05);
    res.mc_95_percentile_low_pnl = terminal_pnls[p05_index];

    // 4. Uncertainty-Adjusted (Fractional) Kelly
    // We penalize the Kelly Fraction if our 95% Confidence Low is vastly
    // different from Expected Pnl Formula: AdjKelly = Kelly * ( 1.0 -
    // (StdDev(MC) / Expected(MC)) ) -- simplistic heuristic penalty

    double variance_sum = 0;
    for (double pnl : terminal_pnls) {
      variance_sum += (pnl - res.mc_expected_pnl) * (pnl - res.mc_expected_pnl);
    }
    double std_dev = std::sqrt(variance_sum / mc_iterations);

    double penalty = 1.0;
    if (res.mc_expected_pnl > 0) {
      penalty = 1.0 - (std_dev / res.mc_expected_pnl);
      if (penalty < 0)
        penalty = 0.0; // Massive variance = 0 bet
      if (penalty > 1.0)
        penalty = 1.0;

      // Standard Half-Kelly is usually safer natively.
      res.uncertainty_adjusted_kelly = (res.kelly_fraction / 2.0) * penalty;
    } else {
      res.uncertainty_adjusted_kelly = 0.0;
    }

    return res;
  }
};

} // namespace quanux::engine::metrics
