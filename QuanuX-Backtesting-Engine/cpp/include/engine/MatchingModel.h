#pragma once

#include "quanux/common/OrderBookL3.h"
#include <vector>

namespace quanux::engine {

using namespace quanux::common;

struct MatchResult {
  bool filled;
  uint32_t fill_qty;
  int64_t fill_price;
  uint64_t timestamp;
  // Forensics
  int32_t queue_rank_at_entry;
  int32_t queue_rank_at_fill;
};

class MatchingModel {
public:
  virtual ~MatchingModel() = default;

  // Called when a new order arrives from the strategy
  virtual void on_new_order(OrderBookL3 &book, L3Order &order) = 0;

  // Called when market data updates the book (did we get filled?)
  // Returns a vector of fills (could be partial)
  virtual std::vector<MatchResult> check_matches(OrderBookL3 &book,
                                                 uint64_t current_time) = 0;
};

} // namespace quanux::engine
