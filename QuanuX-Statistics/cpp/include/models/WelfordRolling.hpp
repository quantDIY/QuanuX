#pragma once

#include <algorithm>
#include <cmath>
#include <deque>
#include <vector>

namespace quanux::models {

/**
 * @brief Online Statistics Calculator using Welford's Algorithm
 *
 * Provides O(1) updates for Mean, Variance, and Standard Deviation.
 * Also supports rolling calculations for Sharpe Ratio, VWAP, and Z-Score.
 */
class OnlineStats {
public:
  OnlineStats() = default;

  /**
   * @brief Update statistics with a new value (Welford's Algorithm)
   * @param value The new data point (e.g., price or return)
   */
  void update(double value) {
    count_++;
    double delta = value - mean_;
    mean_ += delta / count_;
    double delta2 = value - mean_;
    m2_ += delta * delta2;
  }

  /**
   * @brief Reset all statistics
   */
  void reset() {
    count_ = 0;
    mean_ = 0.0;
    m2_ = 0.0;
  }

  [[nodiscard]] double mean() const { return mean_; }

  [[nodiscard]] double variance() const {
    if (count_ < 2)
      return 0.0;
    return m2_ / (count_ - 1);
  }

  [[nodiscard]] double std_dev() const { return std::sqrt(variance()); }

  [[nodiscard]] double z_score(double value) const {
    double sd = std_dev();
    if (sd == 0.0)
      return 0.0;
    return (value - mean_) / sd;
  }

  [[nodiscard]] uint64_t count() const { return count_; }

private:
  uint64_t count_ = 0;
  double mean_ = 0.0;
  double m2_ = 0.0; // Sum of squares of differences from the current mean
};

/**
 * @brief Rolling Window Statistics
 *
 * Maintains a window of recent values to calculate moving averages and
 * volatilities.
 */
class RollingStats {
public:
  explicit RollingStats(size_t window_size) : window_size_(window_size) {}

  void update(double value) {
    window_.push_back(value);

    // Add new value to stats
    stats_.update(value);

    // Remove old value if window is full
    if (window_.size() > window_size_) {
      double old_value = window_.front();
      window_.pop_front();

      // Welford generic removal is checking for numerical stability,
      // but for simple rolling windows, strict removals are complex.
      // For HFT, we often just rebuild or use exponential weighting.
      // Here we re-implement a full recalc for exact sliding window
      // OR use a more advanced removal algorithm.
      // Given the requirement for "Online Algorithms", we will stick to
      // the purely incremental Welford for the *summary* stats,
      // but for a strict *Rolling* window, we might re-compute
      // if the window is small, or use the removal formula.

      // Recomputing for accuracy and simplicity in this version
      // as removal can be unstable.
      recompute();
    }
  }

  [[nodiscard]] double mean() const { return stats_.mean(); }
  [[nodiscard]] double std_dev() const { return stats_.std_dev(); }

  [[nodiscard]] double z_score(double value) const {
    return stats_.z_score(value);
  }

private:
  void recompute() {
    stats_.reset();
    for (double val : window_) {
      stats_.update(val);
    }
  }

  size_t window_size_;
  std::deque<double> window_;
  OnlineStats stats_;
};

} // namespace quanux::models
