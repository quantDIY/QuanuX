#include "market_data_engine.h"
#include <iostream>
#include <thread>

MarketDataEngine::MarketDataEngine(RingBuffer<MarketUpdate, 1024> *ring_buffer,
                                   NatsBridge *nats_bridge)
    : ring_buffer_(ring_buffer), nats_bridge_(nats_bridge) {}

void MarketDataEngine::init(const std::string &extension_path) {
  std::cout << "[MarketDataEngine] Loading extension: " << extension_path
            << std::endl;
  try {
    auto loader = std::make_unique<PluginLoader>(extension_path);

    // In a real implementation, we would get the 'subscribe' and 'start'
    // symbols and register a callback that calls this->on_update

    // Mocking the "start" for now by just keeping the loader alive
    extensions_.push_back(std::move(loader));

    std::cout << "[MarketDataEngine] Successfully loaded " << extension_path
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[MarketDataEngine] Failed to load extension: " << e.what()
              << std::endl;
  }
}

void MarketDataEngine::subscribe(const std::string &symbol) {
  std::cout << "[MarketDataEngine] Subscribing to " << symbol << std::endl;
  // TODO: Iterate extensions and call subscribe
}

void MarketDataEngine::on_update(const MarketUpdate &update) {
  // 1. Push to Strategy RingBuffer (Low Latency)
  if (ring_buffer_) {
    while (!ring_buffer_->push(update)) {
      // Buffer full strategy: Yield? Drop? Spin?
      // In HFT we busy spin or drop.
      // For now, busy spin/yield
      std::this_thread::yield();
    }
  }

  // 2. Push to NATS (Async Sidecar)
  if (nats_bridge_) {
    nats_bridge_->publish_market_data(
        std::to_string(
            update.instrument_id), // Mapping ID to symbol needed in real app
        update.price, update.size, update.is_trade);
  }
}
