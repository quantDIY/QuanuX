#pragma once

#include "engine/FifoMatcher.h"
#include "engine/MatchingModel.h"
#include "quanux/common/OrderBookL3.h"
#include <memory>
#include <queue>
#include <string>
#include <vector>

namespace quanux::engine {
using namespace quanux::common;

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

  struct PendingOrder {
    uint64_t effective_time;
    // Order details would go here, simplified:
    uint64_t id;
    int64_t price;
    uint32_t size;
    bool is_bid;

    bool operator>(const PendingOrder &other) const {
      return effective_time > other.effective_time;
    }
  };

  std::priority_queue<PendingOrder, std::vector<PendingOrder>,
                      std::greater<PendingOrder>>
      pending_orders_;
  std::vector<std::string> fills_; // Hacky fill log for now

  void process_pending_orders(uint64_t until_time_ns) {
    while (!pending_orders_.empty() &&
           pending_orders_.top().effective_time <= until_time_ns) {
      auto po = pending_orders_.top();
      pending_orders_.pop();

      // "Router" to "Exchange" latency done. Now inject into simulated matcher.
      // In real MBO, we would just "Place" it in the book at the end of the
      // queue. For our simulated strategy, we track it separately or just
      // pretend it's a market order if aggressive. Simplified: Add to book as a
      Side side = po.is_bid ? Side::Bid : Side::Ask;
      book_.add(po.id, po.price, po.size, side, po.effective_time);

      // Notify the matcher that a new simulated order arrived
      L3Order new_order = {po.id, po.price, po.size, po.effective_time, side};
      matcher_->on_new_order(book_, new_order);

      // Try match immediately upon arrival
      auto matches = matcher_->check_matches(book_, po.effective_time);
      for (auto &m : matches) {
        fills_.push_back("Fill: " + std::to_string(m.fill_qty) + " @ " +
                         std::to_string(m.fill_price));
      }
    }
    current_time_ns_ = until_time_ns;
  }

  void on_market_data(uint64_t id, int64_t price, uint32_t size, bool is_bid,
                      bool is_add) {
    // Advance time to this event? Or does the runner do that?
    // Let's assume the runner calls process_pending_orders BEFORE calling
    // on_market_data with the event time.

    if (is_add) {
      book_.add(id, price, size, is_bid ? Side::Bid : Side::Ask,
                current_time_ns_);
    } else {
      book_.remove(id);
    }
    auto matches = matcher_->check_matches(book_, current_time_ns_);
    for (auto &m : matches) {
      // Our order might have been matched by this trade!
      fills_.push_back("Fill (Passive): " + std::to_string(m.fill_qty) + " @ " +
                       std::to_string(m.fill_price));
    }
  }

  void on_market_trade(int64_t price, uint32_t size, bool is_aggressive_bid) {
    matcher_->on_market_trade(price, size,
                              is_aggressive_bid ? Side::Bid : Side::Ask);

    // After the trade affects queue position, check matches
    auto matches = matcher_->check_matches(book_, current_time_ns_);
    for (auto &m : matches) {
      fills_.push_back("Fill (Queue Depleted): " + std::to_string(m.fill_qty) +
                       " @ " + std::to_string(m.fill_price));
    }
  }

  void send_order(uint64_t id, int64_t price, uint32_t size, bool is_bid) {
    uint64_t arrival_time = current_time_ns_ + config_.wire_delay_ns;
    pending_orders_.push({arrival_time, id, price, size, is_bid});
  }
};

} // namespace quanux::engine
