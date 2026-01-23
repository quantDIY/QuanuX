#pragma once
#include "nats_bridge.h"
#include "plugin_loader.h"
#include "ring_buffer.h"
#include "strategy_interface.h"
#include <memory>
#include <string>
#include <vector>

// Forward declare to avoid circular dependency if any
class NatsBridge;

class MarketDataEngine {
  std::vector<std::unique_ptr<PluginLoader>> extensions_;
  RingBuffer<MarketUpdate, 1024> *ring_buffer_ =
      nullptr; // Pointer to the shared ring buffer
  NatsBridge *nats_bridge_ = nullptr;

public:
  MarketDataEngine(RingBuffer<MarketUpdate, 1024> *ring_buffer,
                   NatsBridge *nats_bridge);
  void init(const std::string &extension_path);
  void subscribe(const std::string &symbol);
  void on_update(const MarketUpdate &update);
};
