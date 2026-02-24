#pragma once

#include "quanux/sovereign_state.hpp"
#include <atomic>
#include <immintrin.h>

namespace quanux::sentinel {

/**
 * @class BareMetalSentinel
 * @brief The "Adult in the Room" - Enforcing the 5 Natural Laws of QuanuX
 *
 * Binds to Core 5. Evaluates QuanuX-Spreader telemetry via the L3 Cache.
 * Atomically enforces hardware kill-switches. Zero syscalls. Zero
 * de-scheduling.
 */
class BareMetalSentinel {
public:
  explicit BareMetalSentinel(quanux::SovereignState *state) : state_(state) {}

  [[noreturn]] void run_vigil() {
    // Law of Sovereignty (The Anti-Thread Decree): This is the sovereign
    // kingdom of Core 5. We spin endlessly.
    while (true) {
      evaluate_state_machine();
      evaluate_risk();

      // Allow L1 cache warmth while yielding pipeline to prevent branch
      // saturation
      _mm_pause();
    }
  }

private:
  quanux::SovereignState *state_;

  // Sentinel Parameters
  int32_t max_position_limit_{100};
  int32_t dynamic_position_limit_{100};
  double max_notional_limit_{5000000.0}; // $5M hard limit
  uint32_t max_tick_drift_{3000000};     // 1ms drift at 3GHz
  uint8_t hard_order_limit_{20};         // orders per second limit

  inline void evaluate_state_machine() {
    if (!state_)
      return;

    quanux::ExecutionState current_state =
        state_->execution_state.load(std::memory_order_relaxed);

    switch (current_state) {
    case quanux::ExecutionState::PARTIAL:
      // Law of the Witness & The Interlock
      // RUTHLESS BUT SAFE: A partial fill implies liquidity just dried up or
      // toxified. We do not HALT the machine; we instantly constrict the
      // hardware limit. By dynamically setting the max position to exactly what
      // we currently hold, we "re-program" the Spreader's intent via L3 memory.
      // It can no longer average down or scale up (blocked by evaluate_risk),
      // but the hardware remains UNLOCKED for the ExecutionState::HEDGING exit
      // order to fire.
      dynamic_position_limit_ =
          std::abs(state_->current_position.load(std::memory_order_relaxed));
      break;

    case quanux::ExecutionState::HEDGING:
      // We are offloading risk. Maintain the dynamic limit (or relax if
      // needed).
      break;

    case quanux::ExecutionState::HALT:
      trigger_interlock();
      break;

    case quanux::ExecutionState::IDLE:
    case quanux::ExecutionState::WORKING:
    default:
      // Relax constraints back to normal operating parameters
      dynamic_position_limit_ = max_position_limit_;
      break;
    }
  }

  inline void evaluate_risk() {
    if (!state_)
      return;

    // Law of Locality (The 64-Byte Covenant): We read 16 bytes of metadata
    // packed side-by-side with our execution state.
    uint8_t orders = state_->orders_fired.load(std::memory_order_relaxed);

    // 1. The Sin of the Order Storm
    if (orders > hard_order_limit_) {
      state_->execution_state.store(quanux::ExecutionState::HALT,
                                    std::memory_order_relaxed);
      trigger_interlock();
      return;
    }

    int32_t pos = state_->current_position.load(std::memory_order_relaxed);
    int32_t abs_pos = std::abs(pos);

    // 2. The Sin of the Position Breach (or Partial Fill Constriction)
    if (abs_pos > dynamic_position_limit_) {
      state_->execution_state.store(quanux::ExecutionState::HALT,
                                    std::memory_order_relaxed);
      trigger_interlock();
      return;
    }

    uint8_t index_val = state_->tap_index.load(std::memory_order_relaxed);
    if (index_val == 0)
      return; // No telemetry generated yet

    uint8_t head = (index_val - 1) % 3;
    const auto &tap = state_->telemetry_tap[head];

    // 3. The Sin of the Notional Breach
    double notional = abs_pos * tap.best_bid;
    if (notional > max_notional_limit_) {
      state_->execution_state.store(quanux::ExecutionState::HALT,
                                    std::memory_order_relaxed);
      trigger_interlock();
      return;
    }

    // Law of Determinism (The TSC Registry)
    // 4. The Sin of the Stale Tick (Clock Drift)
    uint32_t current_tsc = static_cast<uint32_t>(__builtin_ia32_rdtsc());
    uint32_t tsc_delta = current_tsc - tap.tsc_lo;

    // If the Spreader hasn't updated the tap in >1ms, the exchange link is
    // likely dead or paused. We HALT execution instantly.
    if (tsc_delta > max_tick_drift_) {
      state_->execution_state.store(quanux::ExecutionState::HALT,
                                    std::memory_order_relaxed);
      trigger_interlock();
      return;
    }
  }

  // Law of the Interlock (The Physical Gate)
  [[gnu::always_inline]] inline void trigger_interlock() {
    // The "Homer" Insight: Instead of a standard std::atomic store instruction
    // which may invoke a full `xchg` or heavier cache line flush, we utilize
    // the specific x86 `LOCK BTS` (Bit Test and Set) instruction. This sets the
    // 0th bit directly across the bus without invalidating or looping if it's
    // already set. ABSOLUTELY NO std::cout OR SYSCALLS PERMITTED IN THIS SCOPE.
    uint8_t *ptr = reinterpret_cast<uint8_t *>(&state_->risk_interlock);
    __asm__ volatile("lock btsb $0, %0" : "+m"(*ptr) : : "memory", "cc");
  }
};

} // namespace quanux::sentinel
