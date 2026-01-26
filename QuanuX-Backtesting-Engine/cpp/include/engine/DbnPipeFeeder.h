#pragma once
#include "engine/SimulatedExchange.h"
#include "quanux/common/StrategyInterface.h"
#include <memory>
#include <string>

// Forward declare databento type to avoid leaking heavy include
namespace databento {
class DbnDecoder;
}

namespace quanux::engine {

class DbnPipeFeeder {
  SimulatedExchange *exchange_;
  std::unique_ptr<databento::DbnDecoder> decoder_;
  // Buffer for reading from stdin
  std::vector<uint8_t> read_buffer_;

public:
  explicit DbnPipeFeeder(SimulatedExchange *exchange);
  ~DbnPipeFeeder();

  // Run the feeder loop (blocking)
  void run();
};

} // namespace quanux::engine
