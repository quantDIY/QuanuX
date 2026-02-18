#pragma once

#include "IEngine.hpp"
#include "Metrics/LatencyTracker.hpp"
#include "Recorder/MarketRecorder.hpp"
#include "Risk/RiskEngine.hpp"
#include "Strategy/StrategyLoader.hpp"
#include <iostream>
#include <memory>

namespace QuanuX::ClearStreet {

class ClearStreetAdapter : public IEngineCallback {
public:
  ClearStreetAdapter(const std::string &configPath);
  ~ClearStreetAdapter();

  void run();
  void stop();

  // Hot-Swap Command
  void reloadStrategy();

  // IEngineCallback Implementation
  void onLogon() override;
  void onLogout() override;
  void onExecutionReport(const ExecutionReport &report) override;
  void onMarketDataCheck(const std::string &symbol, double bid,
                         double ask) override;

private:
  std::unique_ptr<IEngine> engine_;
  std::unique_ptr<RiskEngine> risk_;
  std::unique_ptr<MarketRecorder> recorder_;
  std::unique_ptr<StrategyLoader> strategy_;
  std::unique_ptr<LatencyTracker> metrics_;

  bool running_;

  void setupEngine();
};

} // namespace QuanuX::ClearStreet
