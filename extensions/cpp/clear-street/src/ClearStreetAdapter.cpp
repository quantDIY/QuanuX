#include "ClearStreetAdapter.hpp"
#include "Backends/OnixSBackend.hpp"
#include "Backends/QuickFIXFactory.hpp"
#include <chrono>
#include <thread>

namespace QuanuX::ClearStreet {

ClearStreetAdapter::ClearStreetAdapter(const std::string &configPath)
    : running_(false) {
  // 1. Initialize Sub-Components
  metrics_ = std::make_unique<LatencyTracker>();
  // ... (risk, recorder, strategy init unchanged)
  risk_ =
      std::make_unique<RiskEngine>(RiskConfig{}); // Load config in real impl
  recorder_ = std::make_unique<MarketRecorder>(
      "market_data.db", RecorderMode::L3_MARKET_BY_ORDER);
  strategy_ = std::make_unique<StrategyLoader>("./strategies/libstrategy.so");

  // 2. Select Engine
  setupEngine();

  // 3. Connect Components
  if (engine_)
    engine_->setCallback(this);
}

ClearStreetAdapter::~ClearStreetAdapter() { stop(); }

void ClearStreetAdapter::setupEngine() {
#ifdef USE_ONIXS
  std::cout << "Initializing OnixS Backend..." << std::endl;
  engine_ = std::make_unique<OnixSBackend>();
#else
  std::cout << "Initializing QuickFIX Backend (via Factory)..." << std::endl;
  engine_ = createQuickFIXBackend();
#endif
}

void ClearStreetAdapter::run() {
  if (!engine_)
    return;

  // Initialize with config file
  // Secure Credential Injection:
  // We allow the config.ini to contain non-sensitive defaults.
  // The actual Password is injected from the environment variable
  // QUANUX_CLEARSTREET_PASSWORD which is set by quanuxctl (retrieved from
  // Keyring).

  // Note: QuickFIXBackend::init takes a filename.
  // To inject credential, we rely on the QuickFIXBackend specific
  // implementation to check env vars, OR we pass it here if we change the init
  // interface.
  //
  // Since IEngine::init(string configFile) is the interface, let's modify
  // QuickFIXBackend::init to perform this injection logic internally, as it
  // holds the SessionSettings object.
  //
  // However, I can't modify QuickFIXBackend execution logic easily from here
  // without casting.
  //
  // Better approach: Update QuickFIXBackend.hpp to look for the env var in
  // init().

  if (!engine_->init("config.ini")) {
    std::cerr << "Engine Initialization Failed" << std::endl;
    return;
  }

  // Attempt Strategy Load
  if (strategy_->load()) {
    if (auto strat = strategy_->getStrategy()) {
      std::cout << "Strategy Loaded Successfully." << std::endl;
      strat->setEngine(engine_.get());
    }
  } else {
    std::cerr << "Warning: No Strategy loaded." << std::endl;
  }

  engine_->start();
  running_ = true;

  // Keep alive loop
  while (running_) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

void ClearStreetAdapter::stop() {
  running_ = false;
  if (engine_)
    engine_->stop();
}

void ClearStreetAdapter::reloadStrategy() {
  if (strategy_)
    strategy_->reload();
}

// --- Callbacks ---

void ClearStreetAdapter::onLogon() {
  std::cout << "Logon Successful" << std::endl;
}

void ClearStreetAdapter::onLogout() { std::cout << "Logout" << std::endl; }

void ClearStreetAdapter::onExecutionReport(const ExecutionReport &report) {
  metrics_->startMeasure("IngestToLogic");

  // Pass to Risk (update position)
  risk_->onFill(report);

  // Pass to Strategy
  if (auto strat = strategy_->getStrategy()) {
    strat->onExecution(report);
  }

  metrics_->endMeasure("IngestToLogic");
}

void ClearStreetAdapter::onMarketDataCheck(const std::string &symbol,
                                           double bid, double ask) {
  // Archive
  recorder_->recordQuote(
      symbol, bid, ask,
      std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::system_clock::now().time_since_epoch())
          .count());

  // Pass to Strategy
  if (auto strat = strategy_->getStrategy()) {
    strat->onMarketData(symbol, bid, ask);
  }
}

} // namespace QuanuX::ClearStreet
