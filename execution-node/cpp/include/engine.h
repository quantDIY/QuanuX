#pragma once
#include "market_data_engine.h"
#include "nats_bridge.h"
#include "plugin_loader.h"
#include "ring_buffer.h"
#include "strategy_interface.h"
#include <memory>
#include <string>
#include <vector>

struct LoadedStrategy {
  std::unique_ptr<PluginLoader> loader;
  Strategy *strategy;
  StrategyContext *ctx;
};

class Engine {
  RingBuffer<MarketUpdate, 1024> ring_buffer_;
  NatsBridge nats_bridge_;
  MarketDataEngine market_data_engine_;
  std::vector<LoadedStrategy> strategies_;

public:
  Engine();
  void init(const std::string &config_path);
  void load_strategy(const std::string &strategy_path);
  void run();
};
