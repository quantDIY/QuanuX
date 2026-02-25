#pragma once
#ifndef QUANUX_EGRESS_HPP
#define QUANUX_EGRESS_HPP

#include "quanux_synapse.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace quanux {
namespace aether {

/**
 * @brief The Kinetic Egress (Solarflare EF_VI Transmit Organelle)
 *
 * This organelle implements the "Strike".
 * The TCP/UDP templates are pre-built into the SovereignState memory space
 * during the STATE_VIGIL (1) phase. When the Nucleus decides to execute,
 * it does not parse logic, compute sizes, or copy buffers.
 * It merely flips the "Intent Bit" to trigger the `ef_vi_transmit_init()` DMA
 * send.
 */
struct alignas(64) KineticEgress {
  // Cache Line 1: Pre-calculated Network Packet Template
  // Includes Ethernet, IP, and TCP/UDP headers and static payload components.
  // Kept strictly within its own boundary to prevent False Sharing during the
  // Strike.
  uint8_t packet_template[60];
  uint32_t packet_length; // Must total 64 bytes

  // Cache Line 2: The Intent Trigger
  // When the Sovereign Nucleus (Core 3) completes its matrix evaluation
  // and determines an alpha strike condition is met, it asserts this flag.
  alignas(64) std::atomic<bool> intent_bit;

  KineticEgress() : packet_length(0), intent_bit(false) {
    std::memset(packet_template, 0, sizeof(packet_template));
  }

  /**
   * @brief The Law of Pre-Calculation (Ritchie Protocol)
   * To be called strictly during STATE_VOID or STATE_VIGIL.
   * All checksums, routing, and sizing math must be finalized here.
   */
  inline void template_payload(const uint8_t *raw_headers, uint32_t len) {
    // Enforce the 60 byte max limit for direct-hit single-cache-line
    // transmission
    size_t copy_len = (len < 60) ? len : 60;
    std::memcpy(packet_template, raw_headers, copy_len);
    packet_length = static_cast<uint32_t>(copy_len);
  }

  /**
   * @brief The Strike
   * In a live EF_VI environment, flipping this bit prompts the Envoy core
   * to immediately push the pre-loaded Hugepage memory buffer pointer
   * into the ef_vi TX ring.
   */
  inline void fire() { intent_bit.store(true, std::memory_order_release); }
};

static_assert(sizeof(KineticEgress) == 128,
              "KineticEgress MUST map to exactly two 64-byte cache lines.");
static_assert(alignof(KineticEgress) == 64,
              "KineticEgress MUST be 64-byte cache aligned.");

} // namespace aether
} // namespace quanux

#endif // QUANUX_EGRESS_HPP
