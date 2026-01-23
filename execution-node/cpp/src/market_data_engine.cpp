#include "strategy_interface.h"
#include <iostream>
#include <string>
#include <vector>

// Stub for MarketDataEngine
// Responsibilities:
// 1. Load C++ Extensions (quanux_rithmic, quanux_databento)
// 2. Poll/Callback from Extensions
// 3. Push to RingBuffer (for Strategy)
// 4. Push to NatsBridge (for Server)

class MarketDataEngine {
public:
  void init(const std::string &config) {
    std::cout << "[MarketDataEngine] Initializing with config: " << config
              << std::endl;
    // TODO: dlopen() extensions
  }

  void subscribe(const std::string &symbol) {
    std::cout << "[MarketDataEngine] Subscribing to " << symbol << std::endl;
    // TODO: call extension->subscribe(symbol)
  }

  // Callback from Extension
  void on_update(const MarketUpdate &update) {
    // 1. Push to Strategy RingBuffer (Low Latency)
    // ring_buffer.push(update);

    // 2. Push to NATS (Async Sidecar)
    // nats_bridge.publish_market_data(..., update.price, ...);
  }
};
