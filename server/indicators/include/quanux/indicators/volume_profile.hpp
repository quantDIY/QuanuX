#pragma once

#include "quanux/indicators/concepts.hpp"
#include "quanux/indicators/market_profile.hpp" // Reuse Storage Policies

namespace quanux::indicators {

/**
 * @brief Volume Profile Indicator
 * Accumulates volume at price levels using the provided StoragePolicy.
 */
template <typename Policy>
  requires StoragePolicy<Policy>
class VolumeProfile {
private:
  Policy storage_;

public:
  template <typename... Args>
  explicit VolumeProfile(Args &&...args)
      : storage_(std::forward<Args>(args)...) {}

  void process(double price, double volume) { storage_.add(price, volume); }

  double query(double price) { return storage_.get(price); }

  void reset() { storage_.clear(); }
};

} // namespace quanux::indicators
