#include "engine.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

Engine::Engine()
    : // ring_buffer_ default constructor is used (fixed size)
      nats_bridge_("nats://localhost:4222"), // Default NATS URL
      market_data_engine_(&ring_buffer_, &nats_bridge_) {}

void Engine::init(const std::string &config_path) {
  std::cout << "[Engine] Initializing..." << std::endl;
  // In a real app, read config.json

  // Determine platform-specific extension
  std::string ext = ".so";
#ifdef __APPLE__
  ext = ".dylib";
#endif

  // Try multiple paths for the mock feed
  std::vector<std::string> paths = {
      "./mock_feed" + ext, "./execution-node/cpp/build/mock_feed" + ext,
      "../build/mock_feed" + ext};

  bool loaded = false;
  for (const auto &path : paths) {
    try {
      market_data_engine_.init(path);
      loaded = true;
      break;
    } catch (...) {
      // Continue trying
    }
  }

  if (!loaded) {
    // Fallback to error message with the last attempted path's extension logic
    std::cerr << "[Engine] Warning: Could not load mock_feed from standard "
                 "locations (tried "
              << paths[0] << ", etc)." << std::endl;
  }
}

void Engine::load_strategy(const std::string &strategy_path) {
  std::cout << "[Engine] Loading strategy: " << strategy_path << std::endl;
  // Using PluginLoader to load the strategy shared object
  try {
    auto loader = std::make_unique<PluginLoader>(strategy_path);

    // Get the creator function
    auto create_strategy_fn =
        loader->get_symbol<Strategy *(*)()>("create_strategy");
    Strategy *strategy_ptr = create_strategy_fn();

    if (!strategy_ptr) {
      throw std::runtime_error("create_strategy returned null");
    }

    std::cout << "[Engine] Loaded Strategy: " << strategy_ptr->name
              << std::endl;

    // Initialize Strategy
    StrategyContext *ctx = nullptr;
    if (strategy_ptr->create_context) {
      ctx = strategy_ptr->create_context();
    }

    if (strategy_ptr->on_init) {
      strategy_ptr->on_init(ctx);
    }

    strategies_.push_back({std::move(loader), strategy_ptr, ctx});

  } catch (const std::exception &e) {
    std::cerr << "[Engine] Failed to load strategy: " << e.what() << std::endl;
  }
}

void Engine::run() {
  std::cout << "[Engine] Starting Event Loop (Pinned to Core 0)..."
            << std::endl;
  // TODO: Pin thread to core 0

  // Start Market Data Feed (if needed to trigger it)
  market_data_engine_.subscribe("ES.c.0");

  // Core Loop
  bool running = true;
  while (running) {
    MarketUpdate update;
    // Non-blocking pop
    if (ring_buffer_.pop(update)) {
      // Hot Path: Dispatch to all strategies
      for (auto &s : strategies_) {
        if (s.strategy->on_market_data) {
          s.strategy->on_market_data(s.ctx, &update);
        }
      }
    } else {
      // Idle strategy: Busy spin or yield
      // In pure HFT, we busy spin.
      // For dev/test, yield to not burn 100% CPU on laptop
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
  }
}
