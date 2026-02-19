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
  uint64_t local_rec_ts;  // 8 bytes: Nanoseconds since epoch (Packet receipt)
  uint64_t exchange_ts;   // 8 bytes: Nanoseconds since epoch (Matching engine)
  double price;           // 8 bytes: Execution price
  uint32_t size;          // 4 bytes: Execution size
  uint32_t flags;         // 4 bytes: Trade flags
  uint32_t instrument_id; // 4 bytes: Internal instrument mapping

  // Padding to reach 64 bytes
  // Used: 8+8+8+4+4+4 = 36 bytes
  // Remaining: 28 bytes
  uint8_t _pad[28];
};

static_assert(sizeof(MarketTick) == 64, "MarketTick must be exactly 64 bytes");
static_assert(alignof(MarketTick) == 64, "MarketTick must be 64-byte aligned");

} // namespace quanux
