#pragma once
#include "market_data_engine.h"
#include "native_ai_bridge.h"
#include "nats_bridge.h"
#include "order_gateway.h"
#include "plugin_loader.h"
#include "quanux/common/StrategyInterface.h"
#include "ring_buffer.h"
#include <memory>
#include <string>
#include <vector>

using namespace quanux::common;

struct LoadedStrategy {
  std::unique_ptr<PluginLoader> loader;
  Strategy *strategy;
  StrategyContext *ctx;
};

class Engine {
  RingBuffer<MarketUpdate, 1024> ring_buffer_;
  NativeAIBridge ai_bridge_;
  NatsBridge nats_bridge_;
  MarketDataEngine market_data_engine_;
  OrderGateway order_gateway_;
  std::vector<LoadedStrategy> strategies_;

  // Static callbacks for Strategy ABI
  static uint64_t static_submit_order(void *ctx, const OrderRequest *request);
  static void static_cancel_order(void *ctx, uint64_t order_id);
  static bool static_query_ai(void *ctx, const char *prompt, char *buffer,
                              uint32_t buffer_size);

public:
  Engine();
  void init(const std::string &config_path);
  void load_strategy(const std::string &strategy_path);
  void run();
};
