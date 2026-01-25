#pragma once

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <unordered_map>
#include <vector>

namespace quanux::simulator {

enum class Side { Bid, Ask };

struct L3Order {
  uint64_t id;
  int64_t price; // In micros/nanos
  uint32_t size;
  uint64_t timestamp; // ns
  Side side;
};

// A price level in the book
struct Level {
  int64_t price;
  std::vector<L3Order *> queue; // Orders in FIFO order

  uint32_t total_volume() const {
    uint32_t vol = 0;
    for (auto *o : queue)
      vol += o->size;
    return vol;
  }
};

class OrderBookL3 {
public:
  // Hash map for O(1) lookup by ID (MBO)
  std::unordered_map<uint64_t, L3Order> orders_;

  // Ordered Maps for Price Levels
  // Bids: Sorted Descending (Highest buy first)
  std::map<int64_t, Level, std::greater<int64_t>> bids_;
  // Asks: Sorted Ascending (Lowest sell first)
  std::map<int64_t, Level, std::less<int64_t>> asks_;

  void add(uint64_t id, int64_t price, uint32_t size, Side side, uint64_t ts) {
    // 1. Store the order object
    auto &order = orders_[id];
    order = {id, price, size, ts, side};

    // 2. Add pointer to the correct level queue
    if (side == Side::Bid) {
      bids_[price].price = price;
      bids_[price].queue.push_back(&order);
    } else {
      asks_[price].price = price;
      asks_[price].queue.push_back(&order);
    }
  }

  void remove(uint64_t id) {
    auto it = orders_.find(id);
    if (it == orders_.end())
      return;

    L3Order &order = it->second;

    // Remove from Level Queue (Linear scan of queue - usually short for active
    // levels) Optimization: Linked list or intrusive list for O(1) removal
    if (order.side == Side::Bid) {
      auto &q = bids_[order.price].queue;
      std::erase(q, &order);
      if (q.empty())
        bids_.erase(order.price);
    } else {
      auto &q = asks_[order.price].queue;
      std::erase(q, &order);
      if (q.empty())
        asks_.erase(order.price);
    }

    orders_.erase(it);
  }

  // Snapshot Debug
  void print_top_of_book() {
    if (!bids_.empty())
      std::cout << "Bid: " << bids_.begin()->first << " ("
                << bids_.begin()->second.total_volume() << ") | ";
    else
      std::cout << "Bid: EMPTY | ";

    if (!asks_.empty())
      std::cout << "Ask: " << asks_.begin()->first << " ("
                << asks_.begin()->second.total_volume() << ")" << std::endl;
    else
      std::cout << "Ask: EMPTY" << std::endl;
  }
};

} // namespace quanux::simulator
