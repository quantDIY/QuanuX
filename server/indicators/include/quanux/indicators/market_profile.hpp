#pragma once

#include "quanux/indicators/concepts.hpp"
#include <cmath>
#include <concepts>
#include <map>
#include <vector>

namespace quanux::indicators {

// -------------------------------------------------------------------------
// Storage Policies
// -------------------------------------------------------------------------

/**
 * @brief Sparse Storage Policy using std::map
 * Good for unlimited ranges (Crypto)
 */
class SparseStorage {
private:
  std::map<double, double> profile_;
  double tick_size_;

public:
  explicit SparseStorage(double tick_size) : tick_size_(tick_size) {}

  // Constructor that accepts dense params but ignores base_price
  explicit SparseStorage(double /*base_price*/, double tick_size)
      : tick_size_(tick_size) {}

  void add(double price, double value) {
    double level = std::round(price / tick_size_) * tick_size_;
    // casting value to int for TPO if needed, but for generic storage we should
    // keep it double or template it For now, let's assume storage holds doubles
    // to support Volume But wait, SparseStorage is holding `int`. Let's change
    // SparseStorage to hold `double` so it can be used for VolumeProfile too
    profile_[level] += value;
  }

  double get(double price) const {
    double level = std::round(price / tick_size_) * tick_size_;
    auto it = profile_.find(level);
    if (it != profile_.end())
      return it->second;
    return 0.0;
  }

  void clear() { profile_.clear(); }
};

/**
 * @brief Dense Storage Policy using std::vector
 * Good for performance on fixed ranges (Futures)
 */
class DenseStorage {
private:
  std::vector<double> buffer_;
  double base_price_;
  double tick_size_;

public:
  DenseStorage(double base_price, double tick_size, int initial_capacity = 1000)
      : base_price_(base_price), tick_size_(tick_size) {
    buffer_.resize(initial_capacity, 0);
  }

  void add(double price, double value) {
    int index =
        static_cast<int>(std::round((price - base_price_) / tick_size_));
    if (index < 0 || index >= static_cast<int>(buffer_.size())) {
      // Naive resize for prototype.
      // In production, we'd want smart resizing (doubling or shifting)
      // For now, we just ignore out of bounds or expand if simple
      if (index > 0) {
        buffer_.resize(index + 1000, 0);
      } else {
        // Handling negative expansion is tricky with vector offset
        // Ignoring for MVP
        return;
      }
    }
    buffer_[index] += value;
  }

  double get(double price) const {
    int index =
        static_cast<int>(std::round((price - base_price_) / tick_size_));
    if (index >= 0 && index < static_cast<int>(buffer_.size())) {
      return buffer_[index];
    }
    return 0.0;
  }

  void clear() { std::fill(buffer_.begin(), buffer_.end(), 0); }
};

// -------------------------------------------------------------------------
// Market Profile Indicator
// -------------------------------------------------------------------------

template <typename Policy>
  requires StoragePolicy<Policy>
class MarketProfile {
private:
  Policy storage_;

public:
  template <typename... Args>
  explicit MarketProfile(Args &&...args)
      : storage_(std::forward<Args>(args)...) {}

  void process(double price) { storage_.add(price, 1.0); }

  int query(double price) { return static_cast<int>(storage_.get(price)); }

  void reset() { storage_.clear(); }
};

} // namespace quanux::indicators
