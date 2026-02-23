#include "spreader/engine.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

std::atomic<bool> global_stop{false};

void signal_handler(int) { global_stop = true; }

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  std::cout << R"(
   ___                         _  __    __  
  / _ \ _   _  __ _ _ __  _   | | \ \  / /  
 | | | | | | |/ _` | '_ \| |  | |  \ \/ /   
 | |_| | |_| | (_| | | | | |_ | |  / /\ \   
  \__\_\\__,_|\__,_|_| |_|\__,|_| /_/  \_\  
                                            
      QuanuX Spreader Execution Engine      
    )" << std::endl;

  std::cout << "Starting QuanuX-Spreader (59ns Dual-Thread Core)..."
            << std::endl;

  quanux::spreader::DualThreadSpreader engine;
  engine.start();

  // Main thread acts as telemetry supervisor, keeping OS process alive
  while (!global_stop) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  std::cout << "\nStopping QuanuX-Spreader..." << std::endl;
  engine.stop();
  return 0;
}
