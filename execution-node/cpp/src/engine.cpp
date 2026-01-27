#include "engine.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

Engine::Engine()
    : // ring_buffer_ default constructor is used (fixed size)
      // Retrieve AI Config from Env or Default
      // Default Endpoint "" triggers AUTO-DISCOVERY in NativeAIBridge
      ai_bridge_(
          std::getenv("QUANUX_AI_ENDPOINT") ? std::getenv("QUANUX_AI_ENDPOINT")
                                            : "",
          std::getenv("QUANUX_AI_KEY") ? std::getenv("QUANUX_AI_KEY") : "",
          std::getenv("QUANUX_AI_MODEL") ? std::getenv("QUANUX_AI_MODEL")
                                         : "llama3",
          std::getenv("QUANUX_AI_PROVIDER") ? std::getenv("QUANUX_AI_PROVIDER")
                                            : "openai"),
      nats_bridge_("nats://localhost:4222"), // Default NATS URL
      market_data_engine_(&ring_buffer_, &nats_bridge_),
      order_gateway_(&nats_bridge_) {}

uint64_t Engine::static_submit_order(void *ctx, const OrderRequest *request) {
  Engine *engine = static_cast<Engine *>(ctx);
  return engine->order_gateway_.submit_order(request);
}

void Engine::static_cancel_order(void *ctx, uint64_t order_id) {
  Engine *engine = static_cast<Engine *>(ctx);
  engine->order_gateway_.cancel_order(order_id);
}

bool Engine::static_query_ai(void *ctx, const char *prompt, char *buffer,
                             uint32_t buffer_size) {
  Engine *engine = static_cast<Engine *>(ctx);
  // Blocking query
  std::string response = engine->ai_bridge_.query(prompt);
  if (response.size() >= buffer_size) {
    return false; // Buffer too small
  }
  std::strcpy(buffer, response.c_str());
  return true;
}

void Engine::init(const std::string &config_path) {
  std::cout << "[Engine] Initializing..." << std::endl;
  std::cout << "[Engine] AI Bridge Connected: "
            << (ai_bridge_.is_connected() ? "YES" : "NO") << std::endl;

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
      OrderService service = {.engine_ctx = this,
                              .submit_order = static_submit_order,
                              .cancel_order = static_cancel_order,
                              .query_ai = static_query_ai};
      strategy_ptr->on_init(ctx, &service);
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
  if (std::getenv("QUANUX_NATS_FEED")) {
    std::cout << "[Engine] Enabling NATS Feed Consumption..." << std::endl;
    nats_bridge_.subscribe("SIM", [this](const std::string &msg) {
      // Minimal parsing of: {"symbol": "SIM", "price": 4800.5, "size": 1,
      // "type": "trade" ...}
      // TODO: Use real JSON parser.
      try {
        // Quick find
        auto p_pos = msg.find("\"price\": ");
        auto s_pos = msg.find("\"size\": ");
        auto t_pos = msg.find("\"type\": \"");

        if (p_pos != std::string::npos && s_pos != std::string::npos) {
          double price = std::stod(msg.substr(p_pos + 9));
          double size = std::stod(msg.substr(s_pos + 8));
          bool is_trade =
              (msg.find("trade") !=
               std::string::npos); // Correct logic? msg has "type": "trade"

          MarketUpdate update = {
              .timestamp = 0,       // todo
              .instrument_id = 999, // SIM ID
              .price = price,
              .size = size,
              .is_trade = is_trade,
              .side = 1 // unknown
          };
          market_data_engine_.on_update(update);
        }
      } catch (...) {
      }
    });
  } else {
    market_data_engine_.subscribe("ES.c.0");
  }

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

      // Process Orders
      order_gateway_.poll();
    } else {
      // Idle strategy: Busy spin or yield
      // In pure HFT, we busy spin.
      // For dev/test, yield to not burn 100% CPU on laptop
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
  }
}
