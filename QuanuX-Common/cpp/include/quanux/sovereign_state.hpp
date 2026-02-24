#pragma once

#include <atomic>
#include <cstdint>

namespace quanux {

enum class ExecutionState : uint8_t {
  IDLE = 0,
  WORKING = 1,
  PARTIAL = 2,
  HEDGING = 3
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

  // Explicit padding to securely bridge the 16-byte structural boundary
  uint8_t _pad[13];

  // Circular buffer acting as the visual flight recorder tap for UI ingestion
  L3Snapshot telemetry_tap[3];
};

// Compiler-enforced hardware boundary assertions
static_assert(sizeof(SovereignState) == 64,
              "SovereignState MUST BE exactly 64 bytes (1 L3 Cache Line).");
static_assert(alignof(SovereignState) == 64,
              "SovereignState MUST BE correctly aligned to 64 bytes.");

} // namespace quanux
