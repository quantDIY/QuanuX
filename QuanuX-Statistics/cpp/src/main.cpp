#include "stats_engine.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  std::cout << R"(
   ___                         _  __    __  
  / _ \ _   _  __ _ _ __  _   | | \ \  / /  
 | | | | | | |/ _` | '_ \| |  | |  \ \/ /   
 | |_| | |_| | (_| | | | | |_ | |  / /\ \   
  \__\_\\__,_|\__,_|_| |_|\__,|_| /_/  \_\  
                                            
      QuanuX Statistics & Research Node      
    )" << std::endl;

  StatsEngine engine;

  const char *nats_url = std::getenv("QUANUX_NATS_URL");
  const char *db_path = std::getenv("QUANUX_DB_PATH");

  engine.connect_nats(nats_url ? nats_url : "nats://localhost:4222");
  engine.connect_db(db_path ? db_path : "market_stats.duckdb");

  engine.run();

  return 0;
}
