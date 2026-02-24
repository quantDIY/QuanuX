#pragma once

#include <cstdint>
#include <immintrin.h>

namespace quanux::spreader {

/**
 * @brief ClockProvider Interface for Sim-Live Parity
 *
 * To survive Institutional Due Diligence, the execution engine must prove
 * mathematical identicality between Live Trading and Replay Backtests.
 * This interface abstracts the TSC pulse so the Replay Adapter can inject
 * synchronized timestamps, avoiding non-deterministic hardware drift in
 * backtests.
 */
class ClockProvider {
public:
  virtual ~ClockProvider() = default;

  // Abstracted Time Stamp Counter (TSC) read
  virtual uint64_t rdtsc() const = 0;
};

class HardwareClock : public ClockProvider {
public:
  [[gnu::always_inline]] inline uint64_t rdtsc() const override {
    // Direct physical read for Live execution
    return __builtin_ia32_rdtsc();
  }
};

class ReplayClock : public ClockProvider {
public:
  explicit ReplayClock(uint64_t seed_tsc = 0) : injected_tsc_(seed_tsc) {}

  // In Replay mode, the NATS adapter injects the TSC captured from the PCAP
  inline void inject(uint64_t t) { injected_tsc_ = t; }

  [[gnu::always_inline]] inline uint64_t rdtsc() const override {
    return injected_tsc_;
  }

private:
  uint64_t injected_tsc_;
};

} // namespace quanux::spreader
