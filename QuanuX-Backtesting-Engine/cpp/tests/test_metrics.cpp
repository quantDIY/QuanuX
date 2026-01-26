#include "engine/metrics/PerformanceAnalyzer.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

using namespace quanux::engine::metrics;

void test_performance_analyzer() {
  std::cout << "Testing PerformanceAnalyzer..." << std::endl;

  // Scenario:
  // Start $10,000
  // Trade 1: +500 (Risk 100) -> Eq 10500. R=5.
  // Trade 2: -200 (Risk 100) -> Eq 10300. R=-2.
  // Trade 3: +1000 (Risk 100) -> Eq 11300. R=10.

  PerformanceAnalyzer analyzer(10000.0, 0.0);

  // Add trades
  analyzer.addTrade({
    entryPrice : 100,
    exitPrice : 105,
    profit : 500,
    profitPct : 0.05,
    risk : 100,
    durationBars : 1,
    isLong : true
  });
  analyzer.addTrade({
    entryPrice : 100,
    exitPrice : 98,
    profit : -200,
    profitPct : -0.02,
    risk : 100,
    durationBars : 1,
    isLong : true
  });
  analyzer.addTrade({
    entryPrice : 100,
    exitPrice : 110,
    profit : 1000,
    profitPct : 0.1,
    risk : 100,
    durationBars : 1,
    isLong : true
  });

  // Update Equity Curve (simulated daily closes)
  std::vector<double> equity = {
      10000.0, 10200.0, 10500.0, // Trade 1 (Up)
      10400.0, 10300.0,          // Trade 2 (Down)
      10800.0, 11300.0           // Trade 3 (Up)
  };
  analyzer.setEquityCurve(equity);

  auto m = analyzer.calculateMetrics();

  std::cout << "  Net Profit: " << m.netProfit << " (Exp: 1300)" << std::endl;
  assert(m.netProfit == 1300.0);

  std::cout << "  Profit Factor: " << m.profitFactor << " (1500/200 = 7.5)"
            << std::endl;
  assert(std::abs(m.profitFactor - 7.5) < 1e-5);

  std::cout << "  Max Drawdown %: " << m.maxDrawdownPct * 100 << "%"
            << std::endl;
  // Peak 10500 -> 10300. DD = 200/10500 = 0.01904...
  assert(std::abs(m.maxDrawdownPct - 0.0190476) < 1e-4);

  // SQN Check
  // R-Multiples: 5, -2, 10
  // Mean R = (13)/3 = 4.333
  // StdDev R:
  // (5-4.33)^2 = 0.44
  // (-2-4.33)^2 = 40.06
  // (10-4.33)^2 = 32.14
  // SumSq = 72.64, Var = 24.21, StdDev = 4.92
  // SQN = sqrt(3) * (4.333 / 4.92) = 1.732 * 0.88 = 1.52
  std::cout << "  SQN: " << m.sqn << std::endl;
  assert(m.sqn > 1.5 && m.sqn < 1.6); // Approx check

  std::cout << "  Ulcer Index: " << m.ulcerIndex << std::endl;
  std::cout << "  Omega Ratio: " << m.omegaRatio << std::endl;

  std::cout << "PerformanceAnalyzer Passed." << std::endl;
}

int main() {
  test_performance_analyzer();
  std::cout << "All Advanced Metrics Tests Passed." << std::endl;
  return 0;
}
