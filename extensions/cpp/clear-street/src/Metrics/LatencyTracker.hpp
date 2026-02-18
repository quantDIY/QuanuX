#pragma once

#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace QuanuX::ClearStreet {

class LatencyTracker {
public:
  LatencyTracker();

  void startMeasure(const std::string &tag);
  void endMeasure(const std::string &tag);

  void record(const std::string &tag, int64_t micros);

  // Dump histograms to JSON
  std::string dump() const;

private:
  struct Histogram {
    std::atomic<int64_t> count{0};
    std::atomic<int64_t> sum{0};
    std::atomic<int64_t> min{999999999};
    std::atomic<int64_t> max{0};
  };

  std::map<std::string, Histogram> stats_;
  // Basic thread-local storage for start times (could be optimized)
};

} // namespace QuanuX::ClearStreet
