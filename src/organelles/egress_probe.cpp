#include <chrono>
#include <iostream>
#include <vector>

#ifdef __x86_64__
#include <immintrin.h> // For accurate TSC polling
#endif

// The mapped genomes
#include "quanux_egress.hpp"
#include "quanux_synapse.hpp"
#include "sovereign_buffer.hpp"

// Operations: The Kinetic Egress Probe
// Concept: Measure Wire-to-Wire latency (Receive -> Evaluate -> Transmit)
// using the Zero-Copy Ghost Hand-off and Pre-Calculated Strike principles.

using namespace quanux::aether;

inline uint64_t rdtsc() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

void measure_wire_to_wire(int iterations) {
  std::cout << "[Probe] Synthesizing Biological Sovereign Execution Cell..."
            << std::endl;

  // Ingress Setup
  SovereignBuffer<1024> cytoplasm;

  // Egress Setup (Pre-Calculated)
  KineticEgress egress_strike;
  uint8_t spoof_headers[60] = {0x01, 0x02, 0x03, 0x04}; // Simulated Headers

  // STATE_VIGIL: Perform all math here, BEFORE the photon arrives
  egress_strike.template_payload(spoof_headers, sizeof(spoof_headers));

  std::vector<uint64_t> w2w_latency;
  std::cout << "[Probe] Polling for Ingress Photon..." << std::endl;

  for (int i = 0; i < iterations; ++i) {
    // Assume NIC dropped a packet into our ring
    cytoplasm.publish();

    // --- WIRE-TO-WIRE START ---
    uint64_t w2w_start = rdtsc();

    // 1. Ingress: The Ghost Hand-off (Zero-copy read)
    const NeuralSynapse *packet = cytoplasm.poll();
    if (packet) {
      // 2. Evaluate (Simulated Intent check)
      volatile uint32_t val = packet->packet_length;
      (void)val;

      // 3. Egress: The Strike (Flip Intent Bit)
      // No math. No checksums. No memcpy. Just fire.
      egress_strike.fire();
    }

    uint64_t w2w_end = rdtsc();
    // --- WIRE-TO-WIRE END ---

    w2w_latency.push_back(w2w_end - w2w_start);
  }

  uint64_t total = 0;
  for (auto t : w2w_latency)
    total += t;
  std::cout << "[Probe] Average Wire-to-Wire Latency (The Singularity): "
            << (total / iterations) << " TSC Cycles" << std::endl;
}

int main() {
  std::cout << "--- THE KINETIC EGRESS ---" << std::endl;
  std::cout << "Initiating Wire-to-Wire Probe." << std::endl;

  const int iterations = 10000;
  measure_wire_to_wire(iterations);

  std::cout << "--------------------------" << std::endl;
  std::cout << "The Strike is zero-copy. The Singularity holds." << std::endl;
  return 0;
}
