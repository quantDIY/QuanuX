#pragma once
#include "nats_bridge.h"
#include "strategy_interface.h"
#include <atomic>
#include <mutex>
#include <vector>

// OrderGateway handles order submission and lifecycle management.
// It acts as the "Exchange" connector. For Phase 6, it mocks fills.

class OrderGateway {
  NatsBridge *nats_bridge_;
  std::atomic<uint64_t> order_id_counter_{1};

public:
  explicit OrderGateway(NatsBridge *nats_bridge);

  // Callbacks from Strategies
  uint64_t submit_order(const OrderRequest *request);
  void cancel_order(uint64_t order_id);

  // Processing Logic (Called by Engine Loop)
  // In a real system, this would poll the Exchange Gateway
  // In this Mock system, it generates fills
  void poll();
};
