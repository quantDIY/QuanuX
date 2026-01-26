#include "quanux/common/StrategyInterface.h"
using namespace quanux::common;
#include <chrono>
#include <iostream>
#include <thread>

// Mock Feed Implementation
// Simulates an exchange sending ticks

extern "C" {

typedef void (*DataCallback)(const MarketUpdate *);

void subscribe(const char *symbol) {
  std::cout << "[MockFeed] Subscribed to " << symbol << std::endl;
}

void start_feed(DataCallback callback) {
  std::thread([callback]() {
    double price = 100.0;
    while (true) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(100)); // 10 updates/sec

      MarketUpdate update;
      update.timestamp = 0; // TODO: Real timestamp
      update.instrument_id = 1;
      update.price = price;
      update.size = 10;
      update.is_trade = true;

      callback(&update);

      price += (rand() % 3 - 1) * 0.1; // Random walk
    }
  }).detach();
}
}
