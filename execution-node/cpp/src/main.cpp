#include "engine.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  std::cout << "QuanuX HFT Node | Starting..." << std::endl;

  std::string strategy_path = "./ping_pong.so";
  if (argc > 1) {
    strategy_path = argv[1];
  }

  try {
    Engine engine;
    engine.init("config.json");
    engine.load_strategy(strategy_path);

    // Start the engine (blocks)
    engine.run();
  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
