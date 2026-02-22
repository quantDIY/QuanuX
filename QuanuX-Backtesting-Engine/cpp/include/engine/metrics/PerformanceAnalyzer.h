#pragma once
#include <cmath>
#include <limits>
#include <vector>

namespace quanux::engine::metrics {

/**
 * @brief Welford's Algorithm Helper for online Mean/Variance calculation.
 */
class WelfordAccumulator {
public:
  void update(double val) {
    count++;
    double delta = val - mean;
    mean += delta / count;
    double delta2 = val - mean;
    m2 += delta * delta2;

    // For Skew/Kurtosis (requires m3, m4 - omitted for brevity unless needed)
  }

  double get_mean() const { return mean; }
  double get_variance() const { return (count > 1) ? m2 / (count - 1) : 0.0; }
  double get_std_dev() const { return std::sqrt(get_variance()); }
  uint64_t get_count() const { return count; }

private:
  uint64_t count = 0;
  double mean = 0.0;
  double m2 = 0.0;
};

struct Trade {
  double entryPrice;
  double exitPrice;
  double profit;    // Net profit amount
  double profitPct; // Percentage gain/loss
  double risk;      // Initial risk amount (for R-multiples)
  int durationBars; // How long trade was held
  bool isLong;

  // Crucible L3 Execution Metrics
  double mae = 0.0;                     // Maximum Adverse Excursion
  double mfe = 0.0;                     // Maximum Favorable Excursion
  uint32_t queue_position_at_entry = 0; // Predicted L3 queue rank
  double latency_slippage_bps = 0.0;    // Assumed slippage due to network
  uint64_t entry_time_ns = 0;
  uint64_t exit_time_ns = 0;
  uint32_t size = 0;
};

struct Metrics {
  // 1. Profitability
  double netProfit = 0.0;
  double grossProfit = 0.0;
  double grossLoss = 0.0;
  double profitFactor = 0.0;
  double cagr = 0.0;
  double winRate = 0.0;
  double expectancy = 0.0;

  // 2. Risk
  double maxDrawdownPct = 0.0;
  double avgDrawdownPct = 0.0;
  double ulcerIndex = 0.0;
  double annualizedVol = 0.0;

  // 3. Ratios
  double sharpeRatio = 0.0;
  double sortinoRatio = 0.0;
  double calmarRatio = 0.0;
  double omegaRatio = 0.0;
  double informationRatio = 0.0;

  // 4. Statistical
  double sqn = 0.0; // System Quality Number
};

class PerformanceAnalyzer {
private:
  std::vector<double> equityCurve; // Account balance over time
  std::vector<double> returns;     // Period-over-period % returns
  std::vector<Trade> trades;       // List of closed trades
  double riskFreeRate;             // Annualized (e.g., 0.02 for 2%)
  double startingEquity;

public:
  PerformanceAnalyzer(double startingEq = 10000.0, double rfRate = 0.0)
      : startingEquity(startingEq), riskFreeRate(rfRate) {
    equityCurve.push_back(startingEquity);
  }

  void addTrade(const Trade &t) {
    trades.push_back(t);
    // Note: In a real simulation, addTrade would be coupled with equity
    // updates. Here we assume setEquityCurve is called separately for
    // time-series metrics.
  }

  void setEquityCurve(const std::vector<double> &equity) {
    equityCurve = equity;
    calculateReturns();
  }

  // Capture single periodic equity point (streaming)
  void updateEquity(double currentEquity) {
    equityCurve.push_back(currentEquity);
    if (equityCurve.size() > 1) {
      double prev = equityCurve[equityCurve.size() - 2];
      if (prev != 0) {
        returns.push_back((currentEquity - prev) / prev);
      }
    }
  }

  Metrics calculateMetrics(int periodsPerYear = 252) {
    Metrics m;
    if (returns.empty() && trades.empty())
      return m;

    // --- 1. Basic Profitability ---
    for (const auto &t : trades) {
      m.netProfit += t.profit;
      if (t.profit > 0)
        m.grossProfit += t.profit;
      else
        m.grossLoss += std::abs(t.profit);
    }

    m.profitFactor = (m.grossLoss == 0) ? 100.0 : (m.grossProfit / m.grossLoss);

    int wins = 0;
    double sumR = 0.0;
    WelfordAccumulator rMultStats;

    for (const auto &t : trades) {
      if (t.profit > 0)
        wins++;

      // Expectancy & SQN via R-Multiples
      double risk = (t.risk <= 0) ? 1.0 : t.risk; // Avoid div by zero
      double rMultiple = t.profit / risk;
      rMultStats.update(rMultiple);

      // Simple Expectancy ($)
      // (Win% * AvgWin) - (Loss% * AvgLoss) is equivalent to TotalProfit / N
    }

    if (!trades.empty()) {
      m.winRate = (double)wins / trades.size();
      m.expectancy = m.netProfit / trades.size();

      // SQN = sqrt(N) * (ExpectancyR / StdDevR)
      if (rMultStats.get_std_dev() > 0) {
        double expectedR = rMultStats.get_mean();
        m.sqn =
            std::sqrt(trades.size()) * (expectedR / rMultStats.get_std_dev());
      }
    }

    // --- 2. Time-Series Analysis (Returns) ---
    if (!returns.empty()) {
      WelfordAccumulator retStats;
      WelfordAccumulator downsideStats;

      for (double r : returns) {
        retStats.update(r);
        if (r < 0)
          downsideStats.update(r); // Downside deviation usually assumes MAR=0
      }

      double meanRet = retStats.get_mean();
      double vol = retStats.get_std_dev();

      m.annualizedVol = vol * std::sqrt(periodsPerYear);
      double annRet = meanRet * periodsPerYear;

      // Sharpe
      if (m.annualizedVol > 0) {
        m.sharpeRatio = (annRet - riskFreeRate) / m.annualizedVol;
      }

      // Sortino
      // Sortino uses sqrt(sum(min(0, r)^2) / N), Welford approximates variance
      // Let's do a direct pass for Sortino/Downside Dev to be precise
      double downsideSqSum = 0.0;
      for (double r : returns) {
        if (r < 0)
          downsideSqSum += r * r;
      }
      double downsideDev = std::sqrt(downsideSqSum / returns.size());
      double annDownsideDev = downsideDev * std::sqrt(periodsPerYear);

      if (annDownsideDev > 0) {
        m.sortinoRatio = (annRet - riskFreeRate) / annDownsideDev;
      }

      // Omega Ratio (Threshold = 0)
      double sumGain = 0.0;
      double sumLoss = 0.0;
      for (double r : returns) {
        if (r > 0)
          sumGain += r;
        else
          sumLoss += std::abs(r);
      }
      if (sumLoss > 0)
        m.omegaRatio = sumGain / sumLoss;
      else
        m.omegaRatio = 100.0;

      // CAGR
      if (equityCurve.size() > 1) {
        double totalRet = (equityCurve.back() / equityCurve.front());
        double years = (double)returns.size() / periodsPerYear;
        if (years > 0) {
          m.cagr = std::pow(totalRet, 1.0 / years) - 1.0;
        }
      }
    }

    // --- 3. Drawdown Analysis ---
    double peak = -std::numeric_limits<double>::infinity();
    double maxDD = 0.0;
    double sumSqDD = 0.0;
    int ddCount = 0;

    for (double val : equityCurve) {
      if (val > peak)
        peak = val;
      double dd = (peak - val) / peak; // % drawdown

      if (dd > maxDD)
        maxDD = dd;
      sumSqDD += (dd * dd);

      if (dd > 0)
        ddCount++; // Count underwater periods
    }

    m.maxDrawdownPct = maxDD;

    // Ulcer Index: sqrt(mean(DD^2))
    if (!equityCurve.empty()) {
      m.ulcerIndex = std::sqrt(sumSqDD / equityCurve.size());
    }

    // Calmar
    if (m.maxDrawdownPct > 0) {
      m.calmarRatio = m.cagr / m.maxDrawdownPct;
    }

    return m;
  }

private:
  void calculateReturns() {
    returns.clear();
    if (equityCurve.size() < 2)
      return;

    for (size_t i = 1; i < equityCurve.size(); ++i) {
      double prev = equityCurve[i - 1];
      if (prev == 0)
        continue;
      returns.push_back((equityCurve[i] - prev) / prev);
    }
  }
};

} // namespace quanux::engine::metrics
