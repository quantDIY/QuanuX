#pragma once

#include "simulator/FifoMatcher.h"
#include "simulator/MatchingModel.h"
#include "simulator/OrderBookL3.h"
#include <memory>

namespace quanux::simulator {

struct LatencyConfig {
  uint64_t wire_delay_ns = 5000;      // 5us default
  uint64_t matching_engine_ns = 2000; // 2us
};

class SimulatedExchange {
public:
  OrderBookL3 book_;
  LatencyConfig config_;
  std::unique_ptr<MatchingModel> matcher_;
  uint64_t current_time_ns_ = 0;

  SimulatedExchange() {
    // Default to FIFO
    matcher_ = std::make_unique<FifoMatcher>();
  }

  void on_market_data(uint64_t id, int64_t price, uint32_t size, bool is_bid,
                      bool is_add) {
    // Feed external market data into the simulated book state
    if (is_add) {
      book_.add(id, price, size, is_bid ? Side::Bid : Side::Ask,
                current_time_ns_);
    } else {
      book_.remove(id);
    }

    // Check for fills after every book update
    auto fills = matcher_->check_matches(book_, current_time_ns_);
  }

  // TODO: Simulating our OWN order entry
  // void send_order(OrderRequest req);
};

} // namespace quanux::simulator
