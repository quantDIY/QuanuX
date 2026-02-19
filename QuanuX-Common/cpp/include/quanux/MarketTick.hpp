#pragma once

#include <cstdint>

namespace quanux {

/**
 * @brief Cache-aligned Market Tick Structure for HFT
 *
 * Designed to fit exactly into a 64-byte cache line to prevent false sharing
 * and optimize memory access patterns during high-frequency updates.
 */
struct alignas(64) MarketTick {
  uint64_t timestamp; // 8 bytes: Nanoseconds since epoch
  double price;       // 8 bytes: Execution price
  uint32_t size;      // 4 bytes: Execution size
  uint32_t flags;     // 4 bytes: Trade flags (e.g., aggressor side)

  // Padding to reach 64 bytes
  // Current usage: 8 + 8 + 4 + 4 = 24 bytes
  // Remaining: 40 bytes
  uint8_t _pad[40];
};

static_assert(sizeof(MarketTick) == 64, "MarketTick must be exactly 64 bytes");
static_assert(alignof(MarketTick) == 64, "MarketTick must be 64-byte aligned");

} // namespace quanux
