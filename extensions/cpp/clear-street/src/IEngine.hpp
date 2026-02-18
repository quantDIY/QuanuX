#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace QuanuX {
namespace ClearStreet {

// Simple structures for abstraction (mapping to FIX/Internal)
struct OrderRequest {
  std::string symbol;
  std::string side; // "1"=Buy, "2"=Sell
  double quantity;
  double price;
  std::string type; // "1"=Market, "2"=Limit
  std::string clOrdID;
};

struct ExecutionReport {
  std::string orderID;
  std::string execID;
  std::string symbol;
  std::string side;
  double lastQty;
  double lastPx;
  std::string ordStatus;
};

// Callback Interface
class IEngineCallback {
public:
  virtual ~IEngineCallback() = default;
  virtual void onLogon() = 0;
  virtual void onLogout() = 0;
  virtual void onExecutionReport(const ExecutionReport &report) = 0;
  virtual void onMarketDataCheck(const std::string &symbol, double bid,
                                 double ask) = 0;
};

// Abstract Engine Interface
class IEngine {
public:
  virtual ~IEngine() = default;

  virtual void setCallback(IEngineCallback *callback) = 0;
  virtual bool init(const std::string &configFile) = 0;
  virtual void start() = 0;
  virtual void stop() = 0;

  // Trading
  virtual void sendOrder(const OrderRequest &order) = 0;
  virtual void cancelOrder(const std::string &orderID) = 0;

  // Performance Tuning (OnixS specific hints, ignored by QuickFIX)
  virtual void setAffinity(int sendCpu, int recvCpu) {}
  virtual void warmUp() {}
};

} // namespace ClearStreet
} // namespace QuanuX
