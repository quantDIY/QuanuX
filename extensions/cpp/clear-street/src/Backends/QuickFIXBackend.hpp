#pragma once

#include "../IEngine.hpp"
#include <Application.h>
#include <FileLog.h>
#include <FileStore.h>
#include <MessageCracker.h>
#include <Session.h> // Include Session for sendToTarget
#include <SessionSettings.h>
#include <SocketInitiator.h>
#include <cstdlib>
#include <fix44/ExecutionReport.h>
#include <fix44/NewOrderSingle.h>
#include <fix44/OrderCancelRequest.h>
#include <iostream>
#include <set>

namespace QuanuX {
namespace ClearStreet {

class QuickFIXBackend : public IEngine,
                        public FIX::Application,
                        public FIX::MessageCracker {
public:
  QuickFIXBackend() : callback_(nullptr), initiator_(nullptr) {}
  ~QuickFIXBackend() { stop(); }

  // IEngine
  void setCallback(IEngineCallback *callback) override { callback_ = callback; }

  bool init(const std::string &configFile) override {
    try {
      settings_ = std::make_unique<FIX::SessionSettings>(configFile);

      // Secure Credential Injection
      const char *envPwd = std::getenv("QUANUX_CLEARSTREET_PASSWORD");
      if (envPwd) {
        std::set<FIX::SessionID> sessions = settings_->getSessions();
        for (const auto &session : sessions) {
          FIX::Dictionary dict = settings_->get(session);
          dict.setString("Password", envPwd);
          settings_->set(session, dict);
        }
      }

      storeFactory_ = std::make_unique<FIX::FileStoreFactory>(*settings_);
      logFactory_ = std::make_unique<FIX::FileLogFactory>(*settings_);
      initiator_ = std::make_unique<FIX::SocketInitiator>(
          *this, *storeFactory_, *settings_, *logFactory_);
      return true;
    } catch (std::exception &e) {
      std::cerr << "QuickFIX Init Error: " << e.what() << std::endl;
      return false;
    }
  }

  void start() override {
    if (initiator_)
      initiator_->start();
  }

  void stop() override {
    if (initiator_)
      initiator_->stop();
  }

  void sendOrder(const OrderRequest &req) override {
    FIX44::NewOrderSingle newOrder(FIX::ClOrdID(req.clOrdID),
                                   FIX::Side(req.side[0]), // "1" or "2"
                                   FIX::TransactTime(),
                                   FIX::OrdType(req.type[0]) // "1" or "2"
    );
    newOrder.set(FIX::Symbol(req.symbol));
    newOrder.set(FIX::OrderQty(req.quantity));

    if (req.type == "2") { // Limit
      newOrder.set(FIX::Price(req.price));
    }

    // Clear Street required fields? Likely HandlInst logic or similar.
    // For now, minimal standard fields.
    newOrder.set(FIX::HandlInst('1'));
    newOrder.set(FIX::TimeInForce('0')); // Day

    // Send to first active session
    // In production we should track sessions by SenderCompID or Symbol
    if (settings_ && !settings_->getSessions().empty()) {
      FIX::Session::sendToTarget(newOrder, *settings_->getSessions().begin());
    }
  }

  void cancelOrder(const CancelRequest &req) override {
    FIX44::OrderCancelRequest cancelRequest(
        FIX::OrigClOrdID(req.origClOrdID),
        FIX::ClOrdID("C_" + req.origClOrdID), // Simple ID gen
        FIX::Side(req.side[0]), FIX::TransactTime());
    cancelRequest.set(FIX::Symbol(req.symbol));

    if (settings_ && !settings_->getSessions().empty()) {
      FIX::Session::sendToTarget(cancelRequest,
                                 *settings_->getSessions().begin());
    }
  }

  // FIX::Application
  void onCreate(const FIX::SessionID &) override {}
  void onLogon(const FIX::SessionID &) override {
    if (callback_)
      callback_->onLogon();
  }
  void onLogout(const FIX::SessionID &) override {
    if (callback_)
      callback_->onLogout();
  }
  void toAdmin(FIX::Message &, const FIX::SessionID &) override {}

  void toApp(FIX::Message &,
             const FIX::SessionID &) throw(FIX::DoNotSend) override {}

  void fromAdmin(const FIX::Message &,
                 const FIX::SessionID &) throw(FIX::FieldNotFound,
                                               FIX::IncorrectDataFormat,
                                               FIX::IncorrectTagValue,
                                               FIX::RejectLogon) override {}

  void
  fromApp(const FIX::Message &message, const FIX::SessionID &sessionID) throw(
      FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue,
      FIX::UnsupportedMessageType) override {
    crack(message, sessionID);
  }

  // MessageCracker handlers
  void onMessage(const FIX44::ExecutionReport &message,
                 const FIX::SessionID &sessionID) override {
    ExecutionReport report;
    FIX::OrderID orderID;
    if (message.isSet(orderID)) {
      message.get(orderID);
      report.orderID = orderID.getValue();
    }
    FIX::ExecID execID;
    if (message.isSet(execID)) {
      message.get(execID);
      report.execID = execID.getValue();
    }
    FIX::Symbol symbol;
    if (message.isSet(symbol)) {
      message.get(symbol);
      report.symbol = symbol.getValue();
    }
    FIX::Side side;
    if (message.isSet(side)) {
      message.get(side);
      report.side = std::string(1, side.getValue());
    }
    FIX::LastQty lastQty;
    if (message.isSet(lastQty)) {
      message.get(lastQty);
      report.lastQty = lastQty.getValue();
    }
    FIX::LastPx lastPx;
    if (message.isSet(lastPx)) {
      message.get(lastPx);
      report.lastPx = lastPx.getValue();
    }
    FIX::OrdStatus ordStatus;
    if (message.isSet(ordStatus)) {
      message.get(ordStatus);
      report.ordStatus = std::string(1, ordStatus.getValue());
    }

    if (callback_)
      callback_->onExecutionReport(report);
  }

private:
  IEngineCallback *callback_;
  std::unique_ptr<FIX::SocketInitiator> initiator_;
  std::unique_ptr<FIX::SessionSettings> settings_;
  std::unique_ptr<FIX::FileStoreFactory> storeFactory_;
  std::unique_ptr<FIX::FileLogFactory> logFactory_;
};

} // namespace ClearStreet
} // namespace QuanuX
