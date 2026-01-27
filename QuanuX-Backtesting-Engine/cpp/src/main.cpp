#include "engine/BacktestRunner.h"
#include <iostream>

#include <string>

int main(int argc, char **argv) {
  std::cout << "Starting QuanuX Backtester..." << std::endl;

  quanux::engine::BacktestConfig config;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--nats" || arg == "--replay") {
      config.enable_nats = true;
    } else if (arg == "--nats-url" && i + 1 < argc) {
      config.nats_url = argv[++i];
    }
  }

  quanux::engine::BacktestRunner runner;
  runner.run(config);

  return 0;
}
