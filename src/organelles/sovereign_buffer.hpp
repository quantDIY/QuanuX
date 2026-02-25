#pragma once
#ifndef QUANUX_SOVEREIGN_BUFFER_HPP
#define QUANUX_SOVEREIGN_BUFFER_HPP

#include "quanux_synapse.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>

namespace quanux {
namespace aether {

/**
 * @brief The Sovereign Buffer (The Synaptic Bridge)
 *
 * A lock-free, zero-copy ring buffer residing in the quanux-shm Cytoplasm.
 * The Solarflare NIC writes chunks directly into the memory region mapped by
 * this ring via DMA. The Nucleus reads the slots by moving a pointer. Nothing
 * is ever copied.
 */
template <size_t N> class SovereignBuffer {
  static_assert(
      (N & (N - 1)) == 0,
      "SovereignBuffer size N must be a power of 2 for bitwise masking.");

public:
  SovereignBuffer() : head_(0), tail_(0) {}

  // Simulated Envoy DMA interaction
  // In actual production EF_VI, the NIC drops the photons and advances the
  // register natively.
  inline void publish() {
    head_.store(head_.load(std::memory_order_relaxed) + 1,
                std::memory_order_release);
  }

  // Called by the Nucleus (Core 3).
  // This executes "The Ghost Hand-off".
  // We return a pointer to the physical memory structure mapped over the NIC
  // buffer.
  inline const NeuralSynapse *poll() {
    size_t current_tail = tail_.load(std::memory_order_relaxed);

    // Check if the NIC has advanced the head ptr
    if (current_tail < head_.load(std::memory_order_acquire)) {
      const NeuralSynapse *synapse = &ring_[current_tail & (N - 1)];

      // Advance our read pointer
      tail_.store(current_tail + 1, std::memory_order_relaxed);

      return synapse; // Zero-copy. Data stays where the NIC dropped it.
    }
    return nullptr; // No new photons. The wire is silent.
  }

private:
  // The Ring Geometry: Aligned to 64 bytes to prevent False Sharing.
  alignas(64) NeuralSynapse ring_[N];

  // Cache Line 1: Producer (Envoy/NIC). Pad to 64 bytes.
  alignas(64) std::atomic<size_t> head_;

  // Cache Line 2: Consumer (Nucleus/Core 3). Pad to 64 bytes.
  alignas(64) std::atomic<size_t> tail_;
};

} // namespace aether
} // namespace quanux

#endif // QUANUX_SOVEREIGN_BUFFER_HPP
