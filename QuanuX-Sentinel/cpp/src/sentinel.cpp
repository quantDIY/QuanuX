#pragma once

#include "quanux/sovereign_state.hpp"
#include <immintrin.h>

namespace quanux::sentinel {

// The "Adult in the Room"
// Binds to Core 5. Evaluates QuanuX-Spreader telemetry via the L3 Cache.
// Atomically enforces hardware kill-switch. Zero syscalls. Zero de-scheduling.
class BareMetalSentinel {
public:
  explicit BareMetalSentinel(quanux::SovereignState *state) : state_(state) {}

  [[noreturn]] void run_vigil() {
    // Standard loop execution on pinned Core 5
    while (true) {
      evaluate_risk();

      // Allow L1 cache warmth while yielding pipeline to prevent branch
      // saturation
      _mm_pause();
    }
  }

private:
  quanux::SovereignState *state_;

  double current_position_{0.0};
  double max_position_limit_{100.0};
  double max_notional_limit_{5000000.0}; // $5M limit

  inline void evaluate_risk() {
    if (!state_)
      return;

    // 1. Read the latest Telemetry Tap from the Spreader
    // (Pointer dereference is safe; memory mapped permanently to L3)
    uint8_t index_val = state_->tap_index.load(std::memory_order_relaxed);
    if (index_val == 0)
      return; // No telemetry generated yet

    uint8_t head = (index_val - 1) % 3;
    const auto &tap = state_->telemetry_tap[head];

    // 2. Hard Risk Rule: Max Position
    if (current_position_ >= max_position_limit_) {
      trigger_interlock();
      return;
    }

    // 3. Hard Risk Rule: Max Notional
    double notional = current_position_ * tap.best_bid;
    if (notional > max_notional_limit_) {
      trigger_interlock();
      return;
    }

    // 4. Hard Risk Rule: "Stale Data" Detection mapping TSC drift
    uint32_t current_tsc = static_cast<uint32_t>(__builtin_ia32_rdtsc());
    uint32_t tsc_delta = current_tsc - tap.tsc_lo;

    // At 3GHz, 3,000,000 cycles = ~1ms. If the execution node goes dark for
    // >1ms, halt.
    if (tsc_delta > 3000000) {
      trigger_interlock();
      return;
    }
  }

  // Zero-Jitter Interlock Assertion
  [[gnu::always_inline]] inline void trigger_interlock() {
    // We drop a 1 into the L3 cache line. The Spreader picks this up
    // natively on Core 3 inside its `strategy_fix_loop` CMP check.
    // ABSOLUTELY NO std::cout OR SYSCALLS PERMITTED IN THIS SCOPE.
    state_->risk_interlock.store(1, std::memory_order_relaxed);
  }
};

} // namespace quanux::sentinel
