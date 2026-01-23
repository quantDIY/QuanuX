#include "strategy_interface.h"
#include <iostream>

struct StrategyContext {
  int position;
};

extern "C" StrategyContext *create_context() { return new StrategyContext{0}; }

extern "C" void destroy_context(StrategyContext *ctx) { delete ctx; }

void on_init(StrategyContext *ctx) {
  std::cout << "[PingPong] Initialized" << std::endl;
  ctx->position = 0;
}

void on_market_data(StrategyContext *ctx, const MarketUpdate *update) {
  // Simple logic: Buy at 100.5, Sell at 101.0
  if (update->price <= 100.5 && ctx->position == 0) {
    std::cout << "[PingPong] SIGNAL: BUY @ " << update->price << std::endl;
    ctx->position = 1;
    // Signal would be sent here
  } else if (update->price >= 101.0 && ctx->position == 1) {
    std::cout << "[PingPong] SIGNAL: SELL @ " << update->price << std::endl;
    ctx->position = 0;
  }
}

void on_signal(StrategyContext *ctx, const Signal *signal) {
  // Handle external signals
}

void on_order_update(StrategyContext *ctx, const OrderUpdate *update) {
  std::cout << "[PingPong] Order Update: " << update->status << std::endl;
}

void on_destroy(StrategyContext *ctx) {
  std::cout << "[PingPong] Destroyed" << std::endl;
}

extern "C" Strategy *create_strategy() {
  static Strategy s = {.name = "PingPong",
                       .create_context = create_context,
                       .destroy_context = destroy_context,
                       .on_init = on_init,
                       .on_market_data = on_market_data,
                       .on_signal = on_signal,
                       .on_order_update = on_order_update,
                       .on_destroy = on_destroy};
  return &s;
}
