#include "order_gateway.h"
#include <iostream>
#include <thread>

OrderGateway::OrderGateway(NatsBridge *nats_bridge)
    : nats_bridge_(nats_bridge) {}

uint64_t OrderGateway::submit_order(const OrderRequest *request) {
  uint64_t new_id = order_id_counter_.fetch_add(1, std::memory_order_relaxed);

  std::cout << "[OrderGateway] Order Submitted! ID: " << new_id
            << " Side: " << request->side << " Qty: " << request->quantity
            << " Price: " << request->price << std::endl;

  // Async Telemetry
  if (nats_bridge_) {
    // Publish "New Order" event
    // TODO: structured JSON
  }

  // MOCK MATCHING LOGIC (Running in the Strategy's thread context for now, or
  // deferred) For Phase 6, we'll simulate an immediate fill for simplistic
  // verification In reality, this would go to a queue and be picked up by
  // poll() or a separate thread

  // Using simple immediate fill simulation for feedback loop test
  // We need a way to callback the strategy.
  // Wait, OrderGateway doesn't know about strategies directly yet.
  // The Engine needs to route the update back.

  return new_id;
}

void OrderGateway::cancel_order(uint64_t order_id) {
  std::cout << "[OrderGateway] Order Cancelled: " << order_id << std::endl;
}

void OrderGateway::poll() {
  // Check for network acks, fills, etc.
}
