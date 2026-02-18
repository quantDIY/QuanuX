#include "LatencyTracker.hpp"
#include <iostream>
#include <sstream>

namespace QuanuX::ClearStreet {

LatencyTracker::LatencyTracker() {
  // Initialize common tags
  stats_["IngestToLogic"];
  stats_["LogicToSend"];
  stats_["TotalRoundTrip"];
}

void LatencyTracker::startMeasure(const std::string &tag) {
  // In a high-perf system, use TSC or thread_local, avoiding map lookups if
  // possible. Scaffold implementation uses map for clarity.
}

void LatencyTracker::endMeasure(const std::string &tag) {
  // ...
}

void LatencyTracker::record(const std::string &tag, int64_t micros) {
  auto &h = stats_[tag];
  h.count++;
  h.sum += micros;

  int64_t currentMin = h.min;
  while (micros < currentMin &&
         !h.min.compare_exchange_weak(currentMin, micros))
    ;

  int64_t currentMax = h.max;
  while (micros > currentMax &&
         !h.max.compare_exchange_weak(currentMax, micros))
    ;
}

std::string LatencyTracker::dump() const {
  std::stringstream ss;
  ss << "{";
  bool first = true;
  for (const auto &kv : stats_) {
    if (!first)
      ss << ",";
    first = false;

    int64_t c = kv.second.count;
    double avg = c > 0 ? (double)kv.second.sum / c : 0.0;

    ss << "\"" << kv.first << "\": {"
       << "\"count\": " << c << ","
       << "\"min\": " << kv.second.min << ","
       << "\"max\": " << kv.second.max << ","
       << "\"avg\": " << avg << "}";
  }
  ss << "}";
  return ss.str();
}

} // namespace QuanuX::ClearStreet
