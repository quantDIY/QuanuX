#pragma once
#include <atomic>
#include <iostream>
#include <quanux/common/StrategyInterface.h>

using namespace quanux::common;

// Minimal Risk Engine
// Checks:
// 1. Max Order Size
// 2. Max Position (per symbol)

class RiskEngine {
  // Limits
  const double MAX_ORDER_QTY = 10.0;
  const double MAX_POSITION = 50.0;

  // State (Simplification: Global position for now, or per-symbol map)
  // For this POC, we just track a single atomic counter for "Global Net
  // Position"
  std::atomic<double> net_position_{0.0};

public:
  RiskEngine() = default;

  bool check(const OrderRequest *request) {
    // 1. Max Clip Check
    if (std::abs(request->quantity) > MAX_ORDER_QTY) {
      std::cerr << "[RiskEngine] REJECT: Max Order Size exceeded ("
                << request->quantity << " > " << MAX_ORDER_QTY << ")"
                << std::endl;
      return false;
    }

    // 2. Max Position Check (Pre-computation)
    double current_pos = net_position_.load();
    double new_pos = current_pos + (request->quantity * request->side);

    if (std::abs(new_pos) > MAX_POSITION) {
      std::cerr << "[RiskEngine] REJECT: Max Position exceeded (" << new_pos
                << " > " << MAX_POSITION << ")" << std::endl;
      return false;
    }

    return true;
  }

  void on_fill(double quantity, int side) {
    // Update position on fill (not on order submit, to correspond with actual
    // inventory) In conservative risk, we might update on submit (Open
    // Interest). For this demo, let's assume we track "Exposure" (pending +
    // filled). We'll update here simplistically. Actually, to properly enforce
    // pre-trade risk, we should track 'Pending' too. For POC: simple update.
    double change = quantity * side;
    double current = net_position_.load();
    while (!net_position_.compare_exchange_weak(current, current + change))
      ;

    std::cout << "[RiskEngine] Position Updated: " << (current + change)
              << std::endl;
  }
};
