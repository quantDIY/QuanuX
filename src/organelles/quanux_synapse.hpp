#pragma once
#ifndef QUANUX_SYNAPSE_HPP
#define QUANUX_SYNAPSE_HPP

#include <cstdint>

namespace quanux {
namespace aether {

/**
 * @brief The Neural Synapse (Solarflare EF_VI Ring Map)
 *
 * This structure maps exactly to the User-Space memory layout provided by the
 * Solarflare driver. It is not "copied" from the NIC; this struct is simply
 * cast over the physical memory address (2MB Hugepage) where the NIC drops the
 * photons.
 */
struct alignas(64) NeuralSynapse {
  // The physical 64-byte L3 Cache Line alignment boundary prevents False
  // Sharing. False sharing is the Friction of the Unseen. If another core
  // attempts to read adjacent memory, the CPU locks the L3. We strictly isolate
  // the synapse.

  uint64_t tsc_timestamp; // The hardware timestamp of the photon arrival
  uint32_t packet_length; // The raw size of the ingestion

  // The NIC DMA drops the raw bytes directly contiguous to the metadata.
  // We map a 48 byte buffer to complete the 64-byte structural lock.
  uint8_t raw_payload[48];

  // Aligning the payload this way ensures that the Sovereign Sentinel (Core 5)
  // and the Spreader (Core 3) both ingest the entire packet in exactly ONE
  // physical cache line fetch (23ns). No L2 jumps. No page walks.

  // Total Size: 8 + 4 + 48 = 60 bytes. (Leaves 4 bytes for padding to hit
  // exactly 64)
};

static_assert(sizeof(NeuralSynapse) == 64,
              "NeuralSynapse MUST map to exactly one 64-byte cache line.");
static_assert(alignof(NeuralSynapse) == 64,
              "NeuralSynapse MUST be 64-byte cache aligned.");

} // namespace aether
} // namespace quanux

#endif // QUANUX_SYNAPSE_HPP
