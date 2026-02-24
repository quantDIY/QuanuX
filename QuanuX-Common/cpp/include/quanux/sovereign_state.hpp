#pragma once

#include "quanux/ritchie_protocol.hpp"
#include <atomic>
#include <cstdint>

namespace quanux {

enum class ExecutionState : uint8_t {
  STATE_VOID = 0,
  STATE_VIGIL = 1,
  STATE_ENGAGED = 2,
  STATE_HEDGE = 3,
  STATE_HALT = 4,
  STATE_RECOVERY = 5
};

// 16-byte packed snapshot representing the Level 3 Book Tap
struct L3Snapshot {
  float best_bid;
  float best_ask;
  float alpha;
  uint32_t tsc_lo;
};

/**
 * SovereignState: The Memory-Mapped L3 Hardware Contract
 *
 * Strict 64-byte alignment ensuring the Spreader (Core 3), Sentinel (Core 5),
 * and Clerk (Core 7) can synchronize risk interlocking and telemetry taps
 * globally across the CPU cache in a single locked assembly cycle.
 */
struct alignas(64) SovereignState {
  // 0 = Clear, 1 = Halt
  std::atomic<uint8_t> risk_interlock;

  // Core Execution State Sequence
  std::atomic<ExecutionState> execution_state;

  // Ring buffer head pointer for telemetry
  std::atomic<uint8_t> tap_index;

  // Natural Law of the Interlock: Expose states securely in the 16-byte header
  std::atomic<uint8_t> orders_fired;
  std::atomic<int32_t> current_position;

  // Explicit padding to securely bridge the 16-byte structural boundary
  // 1 (risk) + 1 (state) + 1 (tap) + 1 (orders) + 4 (pos) = 8 bytes
  uint8_t _pad[8];

  // Circular buffer acting as the visual flight recorder tap for UI ingestion
  L3Snapshot telemetry_tap[3];
};

// Compiler-enforced hardware boundary assertions
static_assert(sizeof(SovereignState) == 64,
              "SovereignState MUST BE exactly 64 bytes (1 L3 Cache Line).");
static_assert(alignof(SovereignState) == 64,
              "SovereignState MUST BE correctly aligned to 64 bytes.");

} // namespace quanux
