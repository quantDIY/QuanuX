#pragma once

#include "engine/MatchingModel.h"
#include <iostream>
#include <map>
#include <vector>

namespace quanux::engine {

using namespace quanux::common;

class FifoMatcher : public MatchingModel {
public:
  struct TrackedOrder {
    uint64_t id;
    int64_t price;
    uint32_t size;
    uint32_t remaining_size;
    Side side;
    uint64_t timestamp;

    // Core L3 Queue Simulation Metrics
    uint32_t queue_position_at_entry;
    uint32_t volume_ahead;
  };

  std::map<uint64_t, TrackedOrder> tracked_orders_;

  void on_new_order(OrderBookL3 &book, L3Order &order) override {
    // Calculate precise L3 queue position at the moment the order reaches the
    // exchange
    uint32_t volume_ahead = 0;
    if (order.side == Side::Bid) {
      if (book.bids_.count(order.price)) {
        volume_ahead = book.bids_[order.price].total_volume();
      }
    } else {
      if (book.asks_.count(order.price)) {
        volume_ahead = book.asks_[order.price].total_volume();
      }
    }

    tracked_orders_[order.id] = {order.id,     order.price, order.size,
                                 order.size,   order.side,  order.timestamp,
                                 volume_ahead, volume_ahead};
  }

  void on_market_trade(int64_t trade_price, uint32_t trade_size,
                       Side aggressive_side) override {
    // A trade occurred in the live market! We use this to deplete volume_ahead.
    Side passive_side = (aggressive_side == Side::Bid) ? Side::Ask : Side::Bid;

    for (auto &[id, to] : tracked_orders_) {
      if (to.side == passive_side && to.price == trade_price) {
        if (to.volume_ahead >= trade_size) {
          to.volume_ahead -= trade_size;
        } else {
          // The trade ate through the volume ahead of us!
          to.volume_ahead = 0;
        }
      }
    }
  }

  std::vector<MatchResult> check_matches(OrderBookL3 &book,
                                         uint64_t current_time) override {
    std::vector<MatchResult> results;
    std::vector<uint64_t> to_remove;

    for (auto &[id, to] : tracked_orders_) {
      // Very basic match condition: Crosses the spread or volume ahead is
      // depleted.
      bool cross = false;
      if (to.side == Side::Bid && !book.asks_.empty() &&
          to.price >= book.asks_.begin()->first) {
        cross = true;
      } else if (to.side == Side::Ask && !book.bids_.empty() &&
                 to.price <= book.bids_.begin()->first) {
        cross = true;
      }

      if (cross || to.volume_ahead == 0) {
        // Filled!
        MatchResult mr;
        mr.filled = true;
        mr.fill_qty = to.remaining_size;
        mr.fill_price =
            to.price; // Simplified slippage model for this iteration
        mr.timestamp = current_time;
        mr.queue_rank_at_entry = to.queue_position_at_entry;
        mr.queue_rank_at_fill = 0;

        results.push_back(mr);
        to_remove.push_back(id);
      }
    }

    for (uint64_t id : to_remove) {
      tracked_orders_.erase(id);
    }

    return results;
  }
};

} // namespace quanux::engine
