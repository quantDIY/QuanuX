#pragma once
#include <array>
#include <atomic>
#include <cstddef>
#include <immintrin.h> // For _mm_pause
#include <stdexcept>

namespace quanux {

/**
 * @brief Cache-aligned Memory Pool for HFT Hot-Paths.
 * Designed for single-producer, single-consumer allocation patterns.
 * Ensures zero jitter from OS page faults or heap contention.
 */
template <typename T, size_t Capacity> class MemoryPool {
  static_assert(Capacity > 0 && (Capacity & (Capacity - 1)) == 0,
                "Capacity must be a power of 2");

public:
  // Ensure the pool itself starts on a cache line boundary to prevent false
  // sharing
  alignas(64) std::array<T, Capacity> storage;

  MemoryPool() : write_index(0) {
    // Pre-touch memory to avoid page faults during live execution
    for (auto &item : storage) {
      new (&item) T();
    }
  }

  /**
   * @brief Grabs the next available slot in a circular fashion.
   * ZERO allocations. Only ultra-fast pointer math via bitwise AND.
   */
  inline T *next_slot() noexcept {
    size_t idx =
        write_index.fetch_add(1, std::memory_order_relaxed) & (Capacity - 1);
    return &storage[idx];
  }

  /**
   * @brief Reset the pool for the next session.
   */
  void reset() { write_index.store(0, std::memory_order_release); }

private:
  std::atomic<size_t> write_index;
};

} // namespace quanux
