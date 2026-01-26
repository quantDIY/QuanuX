#include "quanux/common/StrategyInterface.h"
#include "quanux/indicators/market_profile.hpp"
using namespace quanux::common;
#include <iostream>
#include <memory>

// Demo Strategy Implementation
class DemoStrategyImpl {
private:
  quanux::indicators::MarketProfile<quanux::indicators::DenseStorage> profile_{
      6900.0, 0.25};
  const OrderService *service_ = nullptr;

public:
  void init(const OrderService *service) {
    service_ = service;
    std::cout << "[DemoStrategy] Initialized.\n";
  }

  void on_market_data(const MarketUpdate *update) {
    if (!update->is_trade)
      return;
    std::cout << "[Demo] Tick: " << update->price << " Sz: " << update->size
              << "\n";

    // Process Indicator
    profile_.process(update->price);
    int tpo = profile_.query(update->price);

    if (tpo > 10) {
      std::cout << "[DemoStrategy] High Traffic at " << update->price
                << " (TPO: " << tpo << ")\n";
    }
  }
};

// -- ABI Boilerplate --
extern "C" {

StrategyContext *demo_create() {
  return reinterpret_cast<StrategyContext *>(new DemoStrategyImpl());
}

// Suppress unused parameter warning
void demo_destroy_ctx(StrategyContext *ctx) {
  delete reinterpret_cast<DemoStrategyImpl *>(ctx);
}

void demo_init(StrategyContext *ctx, const OrderService *service) {
  reinterpret_cast<DemoStrategyImpl *>(ctx)->init(service);
}

void demo_on_market_data(StrategyContext *ctx, const MarketUpdate *update) {
  reinterpret_cast<DemoStrategyImpl *>(ctx)->on_market_data(update);
}

void demo_on_destroy(StrategyContext *ctx) {
  (void)ctx; // Suppress unused parameter warning
  std::cout << "[DemoStrategy] Destroyed.\n";
}

Strategy *create_strategy() {
  static Strategy strategy = {.name = "DemoStrategy",
                              .create_context = demo_create,
                              .destroy_context = demo_destroy_ctx,
                              .on_init = demo_init,
                              .on_market_data = demo_on_market_data,
                              .on_signal = nullptr,
                              .on_order_update = nullptr,
                              .on_destroy = demo_on_destroy};
  return &strategy;
}

} // extern "C"
