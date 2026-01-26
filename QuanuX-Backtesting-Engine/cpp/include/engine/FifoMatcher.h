#pragma once

#include "engine/MatchingModel.h"
#include <vector>

namespace quanux::engine {

using namespace quanux::common;

class FifoMatcher : public MatchingModel {
public:
  void on_new_order(OrderBookL3 &book, L3Order &order) override {
    // In a real simulation, we would snapshot the volume ahead of us here.
    // For now, it's a stub or we assume we are at the back of the queue.
  }

  std::vector<MatchResult> check_matches(OrderBookL3 &book,
                                         uint64_t current_time) override {
    std::vector<MatchResult> results;
    // TODO: Iterate our simulated orders and check if volume ahead has been
    // depleted.
    return results;
  }
};

} // namespace quanux::engine
