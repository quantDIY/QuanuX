#include "engine.h"
#include "simulator/DuckDBFeeder.h"
#include "simulator/SimulatedExchange.h"
#include <iostream>

int main(int argc, char **argv) {
  std::cout << "Starting QuanuX Backtester..." << std::endl;

  quanux::simulator::SimulatedExchange exchange;
  quanux::simulator::DuckDBFeeder feeder(":memory:"); // Or path to db file

  // Test Query
  feeder.conn_.Query("CREATE TABLE ticks (id UBIGINT, price BIGINT, size "
                     "UINTEGER, side VARCHAR, ts UBIGINT)");
  feeder.conn_.Query(
      "INSERT INTO ticks VALUES (1, 5000000000, 10, 'BID', 1000)");

  std::cout << "Backtester initialized. L3 Book ready." << std::endl;

  return 0;
}
