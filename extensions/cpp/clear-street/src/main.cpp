#include "ClearStreetAdapter.hpp"
#include <csignal>
#include <iostream>

using namespace QuanuX::ClearStreet;

std::unique_ptr<ClearStreetAdapter> adapter;

void signalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  if (adapter)
    adapter->stop();
  exit(signum);
}

int main(int argc, char **argv) {
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  std::cout << "Starting QuanuX Clear Street Adapter..." << std::endl;

  // In production, parse argv for config path
  adapter = std::make_unique<ClearStreetAdapter>("config.json");
  adapter->run();

  return 0;
}
