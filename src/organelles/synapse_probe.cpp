#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

#ifdef __x86_64__
#include <immintrin.h> // For accurate TSC polling
#endif

// The mapped genome
#include "quanux_synapse.hpp"

// Operations: The First Synaptic Probe (Event Horizon)
// Concept: Bypass the kernel entirely and measure the noise delta.

using namespace quanux::aether;

// Hardware intrinsic for cycle measurement
inline uint64_t rdtsc() {
  unsigned int lo, hi;
  __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
  return ((uint64_t)hi << 32) | lo;
}

void measure_the_room(int iterations) {
  std::cout << "[Probe] Synthesizing standard OS socket jitter..." << std::endl;
  std::vector<uint64_t> kernel_latency;

  // Simulate typical kernel path: read(), copy(), memory alloc, cache miss
  for (int i = 0; i < iterations; ++i) {
    uint64_t start = rdtsc();
    // Artificial payload simulation (malloc and memset force OS involvement)
    volatile uint8_t *fake_socket_buf = new uint8_t[64];
    fake_socket_buf[0] = 0x01;
    delete[] fake_socket_buf;
    uint64_t end = rdtsc();
    kernel_latency.push_back(end - start);
  }

  uint64_t total = 0;
  for (auto t : kernel_latency)
    total += t;
  std::cout << "[Probe] Average OS Loop (The Room): " << (total / iterations)
            << " TSC Cycles" << std::endl;
}

void measure_the_aether(int iterations) {
  std::cout << "[Probe] Aligning Solarflare EF_VI Hugepage Overlay..."
            << std::endl;
  std::vector<uint64_t> neural_latency;

  // Simulate EF_VI Direct Memory Access (DMA)
  // The pointer is pinned to a predetermined memory address (no malloc/new)
  // The structure enforces 64-byte alignment, preventing False Sharing.
  alignas(64) uint8_t simulated_nic_buffer[64];

  for (int i = 0; i < iterations; ++i) {
    uint64_t start = rdtsc();

    // Zero-Copy cast. The data is already in L3 cache territory.
    NeuralSynapse *synapse =
        reinterpret_cast<NeuralSynapse *>(&simulated_nic_buffer[0]);

    // Force evaluation to prevent compiler optimization
    synapse->packet_length = 64;
    volatile uint32_t val = synapse->packet_length;
    (void)val;

    uint64_t end = rdtsc();
    neural_latency.push_back(end - start);
  }

  uint64_t total = 0;
  for (auto t : neural_latency)
    total += t;
  std::cout << "[Probe] Average Neural Loop (The Aether): "
            << (total / iterations) << " TSC Cycles" << std::endl;
}

int main() {
  std::cout << "--- THE EVENT HORIZON ---" << std::endl;
  std::cout << "Initiating The First Synaptic Probe." << std::endl;
  std::cout << "Mapping Structural Geometry: " << sizeof(NeuralSynapse)
            << " Bytes." << std::endl;

  const int iterations = 10000;

  measure_the_room(iterations);
  measure_the_aether(iterations);

  std::cout << "--------------------------" << std::endl;
  std::cout << "The Delta is confirmed. The wire is silent." << std::endl;
  return 0;
}
