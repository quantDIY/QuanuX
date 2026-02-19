
#include "quanux/MarketTick.hpp"
#include "quanux/SPSCQueue.hpp"
#include <atomic>
#include <chrono>
#include <immintrin.h>
#include <iostream>
#include <thread>
#include <vector>

using namespace quanux;

// Signal simplified for benchmark
struct Signal {
  uint64_t tick_ts;
  uint32_t instrument_id;
  double z_score;
  double volatility;
};

int main() {
  SPSCQueue<Signal> queue(4096);
  std::atomic<bool> running{true};
  std::vector<uint64_t> latencies;
  latencies.reserve(1000000);

  // Consumer Thread (Execution)
  std::thread consumer([&]() {
    Signal sig;
    while (running) {
      if (queue.pop(sig)) {
        auto now = std::chrono::steady_clock::now();
        auto tick_time = std::chrono::steady_clock::time_point(
            std::chrono::nanoseconds(sig.tick_ts));
        auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           now - tick_time)
                           .count();
        latencies.push_back(latency);
      } else {
        _mm_pause();
      }
    }
  });

  // Producer Loop (Simulation)
  std::cout << "Running Micro-Benchmark (1M messages)..." << std::endl;
  auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < 1000000; ++i) {
    auto now = std::chrono::steady_clock::now();
    uint64_t ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      now.time_since_epoch())
                      .count();
    Signal sig{ts, 1, 3.0, 0.5}; // Always trigger
    while (!queue.push(sig)) {
      _mm_pause(); // Spin with pause
    }
  }

  // Wait for drain
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  running = false;
  consumer.join();
  auto end = std::chrono::steady_clock::now();

  std::cout << "Done." << std::endl;

  // Calculate Stats
  uint64_t sum = 0;
  uint64_t min_lat = -1;
  uint64_t max_lat = 0;
  for (auto lat : latencies) {
    sum += lat;
    if (lat < min_lat)
      min_lat = lat;
    if (lat > max_lat)
      max_lat = lat;
  }

  if (!latencies.empty()) {
    std::cout << "Avg Latency: " << (sum / latencies.size()) << " ns"
              << std::endl;
    std::cout << "Min Latency: " << min_lat << " ns" << std::endl;
    std::cout << "Max Latency: " << max_lat << " ns" << std::endl;
    std::cout << "Throughput: "
              << (1000000.0 /
                  std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                        start)
                      .count() *
                  1000)
              << " msg/sec" << std::endl;
  }

  return 0;
}
