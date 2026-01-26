#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "databento_feed.hpp"
#include "dbn_file_feed.hpp"
#include "plugin_loader.h"
#include "rithmic_feed.hpp" // Added for DBN file replay
#include "strategy_interface.h"

// Global shutdown flag
volatile bool keep_running = true;

void signal_handler(int) { keep_running = false; }

void print_usage(const char *prog) {
  std::cout << "Usage: " << prog << " --strategy <path.so> [options]\n";
  std::cout << "Options:\n";
  std::cout << "  --key <API_KEY>         Databento API Key (Live)\n";
  std::cout << "  --dataset <DATASET>     Dataset (e.g., GLBX.MDP3)\n";
  std::cout << "  --symbol <SYMBOL>       Symbol (e.g., ESH5)\n";
  std::cout << "  --file <PATH>           Replay from DBN file (Offline)\n";
}

int main(int argc, char **argv) {
  // Defaults
  std::string strategy_path;
  std::string api_key;
  std::string dataset = "GLBX.MDP3";
  std::string symbol = "ESH6";
  std::string file_path;

  // Rithmic Args
  std::string feed_type = "databento"; // databento, file, rithmic
  std::string r_user, r_pass, r_exch = "CME";
  int start_time = 0;
  int end_time = 0;

  // Simple Arg Parsing
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--strategy" && i + 1 < argc)
      strategy_path = argv[++i];
    else if (arg == "--key" && i + 1 < argc)
      api_key = argv[++i];
    else if (arg == "--dataset" && i + 1 < argc)
      dataset = argv[++i];
    else if (arg == "--symbol" && i + 1 < argc)
      symbol = argv[++i];
    else if (arg == "--file" && i + 1 < argc) {
      file_path = argv[++i];
      feed_type = "file";
    }
    // Rithmic Flags
    else if (arg == "--feed" && i + 1 < argc)
      feed_type = argv[++i];
    else if (arg == "--ruser" && i + 1 < argc)
      r_user = argv[++i];
    else if (arg == "--rpass" && i + 1 < argc)
      r_pass = argv[++i];
    else if (arg == "--start" && i + 1 < argc)
      start_time = std::stoi(argv[++i]);
    else if (arg == "--end" && i + 1 < argc)
      end_time = std::stoi(argv[++i]);
  }

  // Backwards compat: if key set but no feed arg, implies databento. If file
  // set, implies file.

  if (strategy_path.empty()) {
    std::cerr << "Usage: " << argv[0] << " --strategy <path.so> [options]\n"
              << "Options:\n"
              << "  --key <API_KEY>         Databento API Key (Live)\n"
              << "  --file <PATH>           Replay from DBN file (Offline)\n"
              << "  --feed rithmic          Use Rithmic (requires --ruser, "
                 "--rpass)\n";
    return 1;
  }

  std::unique_ptr<Feed> feed; // Use a base Feed pointer

  if (feed_type == "file") {
    // File Mode
    std::cout << "[Runner] Running in File Replay Mode: " << file_path << "\n";
    feed = std::make_unique<DbnFileFeed>(file_path);
  } else if (feed_type == "rithmic") {
    std::cout << "[Runner] Running in Rithmic Mode.\n";
    if (r_user.empty() || r_pass.empty()) {
      // Try env vars
      const char *env_u = std::getenv("QUANUX_RITHMIC_USER");
      const char *env_p = std::getenv("QUANUX_RITHMIC_PASS");
      if (env_u)
        r_user = env_u;
      if (env_p)
        r_pass = env_p;
    }
    if (r_user.empty()) {
      std::cerr << "Error: Rithmic User required (--ruser or "
                   "QUANUX_RITHMIC_USER env var).\n";
      return 1;
    }
    if (r_pass.empty()) {
      std::cerr << "Error: Rithmic Password required (--rpass or "
                   "QUANUX_RITHMIC_PASS env var).\n";
      return 1;
    }
    feed.reset(
        new RithmicFeed(r_user, r_pass, symbol, r_exch, start_time, end_time));
  } else {
    // Default Databento Live
    std::cout << "[Runner] Running in Databento Live Mode.\n";
    if (api_key.empty()) {
      // Try env var
      const char *env_key = std::getenv("DATABENTO_API_KEY");
      if (env_key)
        api_key = env_key;
    }

    if (api_key.empty()) {
      std::cerr << "Error: API Key required for Live Mode (--key or "
                   "DATABENTO_API_KEY). Use --file for Replay.\n";
      return 1;
    }
    feed = std::make_unique<DatabentoFeed>(api_key, dataset, symbol);
  }

  try {
    // 1. Load Strategy
    std::cout << "[Runner] Loading strategy: " << strategy_path << "...\n";
    PluginLoader loader(strategy_path);

    // Define function pointer type for factory
    using CreateStratFn = Strategy *(*)();

    // Use generic dlsym via PluginLoader
    // Note: Our PluginLoader is templated on get_symbol<T>
    // But we need to conform to its API.
    // Let's assume the strategy exports "create_strategy"
    // Wait, strategy_interface.h says:
    /*
      struct Strategy { ... };
       // Strategies must export this function
       // Strategy* create_strategy();
    */

    auto create_fn = loader.get_symbol<CreateStratFn>("create_strategy");
    Strategy *strat = create_fn();

    if (!strat) {
      std::cerr << "Error: Failed to create strategy instance.\n";
      return 1;
    }

    std::cout << "[Runner] Initialized Strategy: " << strat->name << "\n";

    // 2. Connect Feed
    feed->connect();

    // 3. Run Loop
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // TODO: Pass shutdown signal to feed if possible or handle in loop?
    // Feed::start is blocking.
    // We rely on feed->stop() called from signal handler or inside callback?
    // Signal handler is generic void(int). We can't access `feed` easily unless
    // global. For now, let's rely on basic SIGINT killing the process or Feed
    // handling internal stop. Actually, we can check keep_running in the
    // callback and call feed->stop().

    std::cout << "[Runner] Starting...\n";

    feed->start([&](const MarketUpdate &tick) {
      // HOT PATH
      // Bridge Feed -> Strategy
      // We need to bridge the C++ interface.
      // Strategy expects `OnMarketDataFn`.
      // But the methods are function pointers in the struct.

      if (strat->on_market_data) {
        // We need a dummy context for now since we haven't built the full
        // context system
        strat->on_market_data(nullptr, &tick);
      }

      if (!keep_running) {
        feed->stop();
      }
    });

    std::cout << "[Runner] Finished.\n";

    // Cleanup
    if (strat->on_destroy) {
      strat->on_destroy(nullptr);
    }

  } catch (const std::exception &e) {
    std::cerr << "Fatal Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
