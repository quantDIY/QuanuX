#include "engine/BacktestRunner.h"
#include "engine/DbnPipeFeeder.h"
#include "engine/NatsReplayer.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace quanux::engine {

void BacktestRunner::run() {
  std::cout << "BacktestRunner: Initializing Simulation..." << std::endl;

  // 1. Init Components
  DuckDBFeeder feeder; // Assume default or passed args
  SimulatedExchange exchange;
  NatsReplayer replayer; // Connects to localhost:4222

  // 2. Load Strategy (TODO: Dynamic loading)
  std::cout << "BacktestRunner: Loading Strategy..." << std::endl;

  // 3. Run Loop
  // For now, defaulting to Pipe Feeder as requested by user
  // In production, we'd check flags or isatty(STDIN_FILENO)

  std::cout << "BacktestRunner: Starting Pipe Feeder (Stdin)..." << std::endl;
  DbnPipeFeeder pipe_feeder(&exchange);
  pipe_feeder.run();

  /* Replay Logic (Disabled in favor of Pipe for this demo)
  std::cout << "BacktestRunner: Starting Replay..." << std::endl;
  for (int i = 0; i < 100; i++) {
      uint64_t now = 1000000 + i * 1000;
      double price = 5000.0 + (i % 10);
      exchange.on_market_data(i, (int64_t)(price * 1000000), 10, true, true);
      replayer.publish_tick("ES", now, price, 10, true);
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  */

  std::cout << "BacktestRunner: Simulation Complete." << std::endl;
}

} // namespace quanux::engine
