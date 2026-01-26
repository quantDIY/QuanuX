#pragma once
#include <cstdint>

// Pure C ABI for maximum compatibility and stability.
// Strategies implement this interface to be loaded by the engine.

extern "C" {

namespace quanux {
namespace common {

// Data Types (Simplified for ABI stability)
struct MarketUpdate {
  uint64_t timestamp;
  uint64_t instrument_id;
  double price;
  double size;
  bool is_trade; // true = trade, false = quote
  int side;      // 1=Buy/Bid, -1=Sell/Ask
};

struct Signal {
  uint64_t timestamp;
  uint64_t instrument_id;
  int action; // 1=Buy, -1=Sell, 0=None
  double price;
  double quantity;
};

struct OrderUpdate {
  uint64_t timestamp;
  uint64_t order_id;
  uint64_t instrument_id;
  int status; // 0=Pending, 1=Filled, 2=Cancelled, 3=Rejected
  double fill_price;
  double fill_quantity;
  double remaining_quantity;
};

struct OrderRequest {
  uint64_t instrument_id;
  double price;
  double quantity;
  int side; // 1=Buy, -1=Sell
  int type; // 0=Limit, 1=Market
};

// Interface for Strategy to call back into Engine
struct OrderService {
  void *engine_ctx;
  uint64_t (*submit_order)(void *engine_ctx, const OrderRequest *request);
  void (*cancel_order)(void *engine_ctx, uint64_t order_id);
};

// V-Table for Strategy instance
struct StrategyContext;

typedef StrategyContext *(*OnCreateContextFn)();
typedef void (*OnDestroyContextFn)(StrategyContext *ctx);
typedef void (*OnInitFn)(StrategyContext *ctx, const OrderService *service);
typedef void (*OnMarketDataFn)(StrategyContext *ctx,
                               const MarketUpdate *update);
typedef void (*OnSignalFn)(StrategyContext *ctx, const Signal *signal);
typedef void (*OnOrderUpdateFn)(StrategyContext *ctx,
                                const OrderUpdate *update);
typedef void (*OnDestroyFn)(StrategyContext *ctx);

struct Strategy {
  const char *name;
  OnCreateContextFn create_context;
  OnDestroyContextFn destroy_context;
  OnInitFn on_init;
  OnMarketDataFn on_market_data;
  OnSignalFn on_signal;
  OnOrderUpdateFn on_order_update;
  OnDestroyFn on_destroy;
};

} // namespace common
} // namespace quanux

} // extern "C"
