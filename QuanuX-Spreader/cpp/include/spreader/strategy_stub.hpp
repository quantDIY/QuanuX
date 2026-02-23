#pragma once

#include "quanux/MarketTick.hpp"

namespace quanux::spreader::strategy {

/**
 * @brief Default Strategy Stub.
 * This file is meant to be overridden by the quanuxctl spreader package command
 * at compile time via the INJECTED_STRATEGY_HEADER macro.
 */
struct StrategyState {
  // Strategy-specific state (e.g. LOCF arrays, entry thresholds)
};

class InjectionStub {
public:
  static inline void init(StrategyState &state) {
    // Initialization logic for the math layer
  }

  /**
   * @brief The core mathematical evaluation loop.
   * MUST be always_inline to ensure execution within the 59ns hot-path.
   * ZERO allocations. Operates directly on the cache-aligned MarketTick.
   */
  [[gnu::always_inline]] static inline bool
  calculate(StrategyState &state, const quanux::MarketTick &tick) {
    // Evaluate the tick against the strategy logic here.
    // Return true if an execution event should be triggered.
    return false;
  }
};

using ActiveStrategy = InjectionStub;

} // namespace quanux::spreader::strategy
