#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <immintrin.h>

namespace quanux::spreader {

/**
 * @brief Represents a single cached instrument state.
 * Padded to exactly 16 bytes to ensure array elements don't strangely straddle
 * cache lines.
 */
struct alignas(16) PriceEntry {
  double price{0.0};

  // "Dirty Bit" / Sequence Counter
  // Allows Thread 2 to instantly verify if this leg updated without traversing
  // the SPSC queue history
  std::atomic<uint32_t> update_seq{0};

  uint32_t arrival_tsc{
      0}; // Telemetry pulse utilizing the 16-byte alignment space
};

static_assert(sizeof(PriceEntry) == 16, "PriceEntry must be 16 bytes");

/**
 * @brief HFT-Grade Price Matrix for LOCF (Last Observation Carried Forward).
 * Replaces std::map<string, double> with direct array indexing.
 *
 * Target latency: 1 CPU cycle for retrieval.
 */
template <size_t MaxInstruments = 8192> class alignas(64) PriceMatrix {
public:
  std::array<PriceEntry, MaxInstruments> entries;

  PriceMatrix() {
    // Pre-touch memory to avoid page faults on startup
    for (auto &entry : entries) {
      entry.price = 0.0;
      entry.update_seq.store(0, std::memory_order_relaxed);
    }
  }

  /**
   * @brief Updates the LOCF price array.
   * ZERO bounds checking for maximum speed. instrument_id MUST be sanitized
   * before hitting this loop.
   */
  [[gnu::always_inline]] inline void update_price(uint32_t instrument_id,
                                                  double new_price,
                                                  uint32_t tsc_pulse) noexcept {
    entries[instrument_id].price = new_price;
    entries[instrument_id].arrival_tsc = tsc_pulse;
    // memory_order_release ensures the price and telemetry are visible before
    // the sequence increments
    entries[instrument_id].update_seq.fetch_add(1, std::memory_order_release);
  }

  /**
   * @brief Fetches the current LOCF price.
   */
  [[gnu::always_inline]] inline double
  get_price(uint32_t instrument_id) const noexcept {
    return entries[instrument_id].price;
  }

  /**
   * @brief Fetches the dirty bit / update sequence.
   */
  [[gnu::always_inline]] inline uint32_t
  get_seq(uint32_t instrument_id) const noexcept {
    return entries[instrument_id].update_seq.load(std::memory_order_acquire);
  }
};

} // namespace quanux::spreader
