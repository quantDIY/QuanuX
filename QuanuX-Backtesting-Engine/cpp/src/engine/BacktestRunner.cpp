#include "engine/BacktestRunner.h"
#include "engine/DbnPipeFeeder.h"
#include "engine/NatsReplayer.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace quanux::engine {

void BacktestRunner::run(const BacktestConfig &config) {
  std::cout << "BacktestRunner: Initializing Simulation..." << std::endl;

  // 1. Init Components
  SimulatedExchange exchange;
  std::unique_ptr<NatsReplayer> replayer = nullptr;

  if (config.enable_nats) {
    std::cout << "BacktestRunner: NATS Replay Enabled (" << config.nats_url
              << ")" << std::endl;
    replayer = std::make_unique<NatsReplayer>(config.nats_url);
  }

  // 2. Load Strategy (TODO: Dynamic loading)
  std::cout << "BacktestRunner: Loading Strategy..." << std::endl;

  // 3. Run Loop
  std::cout << "BacktestRunner: Starting Pipe Feeder (Stdin)..." << std::endl;
  DbnPipeFeeder pipe_feeder(&exchange, replayer.get());
  pipe_feeder.run();

  std::cout << "BacktestRunner: Simulation Complete." << std::endl;
}

} // namespace quanux::engine
