#pragma once
#include "engine/DuckDBFeeder.h"
#include "engine/SimulatedExchange.h"

namespace quanux::engine {

struct BacktestConfig {
  bool enable_nats = false;
  std::string nats_url = "nats://localhost:4222";
};

class BacktestRunner {
public:
  void run(const BacktestConfig &config);
};

} // namespace quanux::engine
