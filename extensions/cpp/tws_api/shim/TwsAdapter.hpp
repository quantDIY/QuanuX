#pragma once

#include "../../common/include/QuanuX/Common.hpp" // Relative path for internal include
#include <QuanuX/StrategyInterface.hpp>
#include <iostream>
#include <string>

// MOCK TWS SDK Types for Linter/Compilation without SDK
// In a real build, the SDK headers would be in the path.
#ifndef TWS_SDK_AVAIL
using TickerId = long;
using TickType = int;
struct TickAttrib {
  int AutoExecute;
};
struct Contract {
  long conId;
  std::string symbol;
  std::string secType;
  std::string lastTradeDateOrContractMonth;
  double strike;
  std::string right;
  std::string multiplier;
  std::string exchange;
  std::string primaryExchange;
  std::string currency;
  std::string localSymbol;
  std::string tradingClass;
  bool includeExpired;
  std::string secIdType;
  std::string secId;
  std::string comboLegsDescrip;
};
struct Order {
  long orderId;
  long clientId;
  long permId;
  std::string action;
  double totalQuantity;
  std::string orderType;
  double lmtPrice;
  double auxPrice;
  std::string tif;
  std::string ocaGroup;
  std::string account;
  std::string openClose;
  int origin;
  std::string orderRef;
  bool transmit;

  // TWS Order has many fields, just adding basics for adapter
};

// Mock EWrapper/EClientSocket
class EWrapper {
public:
  virtual ~EWrapper() = default;
  virtual void tickPrice(TickerId tickerId, TickType field, double price,
                         const TickAttrib &attrib) = 0;
  virtual void tickSize(TickerId tickerId, TickType field, int size) = 0;
  virtual void error(int id, int errorCode, const std::string &errorString) = 0;
};
class EReaderOSSignal {
public:
  EReaderOSSignal(int timeout) {}
};
class EClientSocket {
public:
  EClientSocket(EWrapper *w, EReaderOSSignal *s) {}
  bool eConnect(const char *host, int port, int clientId) { return true; }
  void placeOrder(int id, const Contract &c, const Order &o) {}
};
#endif

// If proper SDK is injected, we'd include its headers instead:
// #include "EWrapper.h"
// #include "EClientSocket.h"
// #include "Contract.h"
// #include "Order.h"

namespace QuanuX {
namespace TwsApi {

class TwsAdapter : public QuanuX::IExecutionProvider, public EWrapper {
public:
  TwsAdapter(const std::string &host, int port, int clientId)
      : osSignal_(2000), client_(this, &osSignal_), host_(host), port_(port),
        clientId_(clientId) {}

  bool connect() { return client_.eConnect(host_.c_str(), port_, clientId_); }

  virtual void sendOrder(const QuanuX::Order &order) override {
    // Map QuanuX order to TWS Order/Contract
    Contract contract;
    contract.symbol = order.symbol;
    contract.secType = "STK"; // Default, needs mapping logic
    contract.exchange = "SMART";
    contract.currency = "USD";

    ::Order twsOrder;
    twsOrder.action = (order.side == QuanuX::Side::Buy) ? "BUY" : "SELL";
    twsOrder.totalQuantity = order.quantity;
    twsOrder.orderType = "LMT";
    twsOrder.lmtPrice = order.price;

    client_.placeOrder(order.id, contract, twsOrder);
  }

  // EWrapper methods (stubs)
  void tickPrice(TickerId tickerId, TickType field, double price,
                 const TickAttrib &attrib) override {}
  void tickSize(TickerId tickerId, TickType field, int size) override {}
  void error(int id, int errorCode, const std::string &errorString) override {}
  // ... many other virtual methods needed ...

private:
  EReaderOSSignal osSignal_;
  EClientSocket client_;
  std::string host_;
  int port_;
  int clientId_;
};
} // namespace QuanuX::TwsApi
