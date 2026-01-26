#include "engine/BacktestRunner.h"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Starting QuanuX Backtester..." << std::endl;

  quanux::engine::BacktestRunner runner;
  runner.run();

  return 0;
}
