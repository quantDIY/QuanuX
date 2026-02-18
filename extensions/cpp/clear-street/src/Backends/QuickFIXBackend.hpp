#pragma once

#include "../IEngine.hpp"
#include <Application.h>
#include <FileLog.h>
#include <FileStore.h>
#include <MessageCracker.h>
#include <SessionSettings.h>
#include <SocketInitiator.h>
#include <cstdlib>
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
    // Implementation map to FIX::Message (NewOrderSingle)
    // ...
  }

  void cancelOrder(const std::string &orderID) override {
    // ...
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
    FIX::MessageCracker::crack(message, sessionID);
  }

  // MessageCracker handlers (e.g. onMessage(const FIX42::ExecutionReport&,
  // ...))
  // ...

private:
  IEngineCallback *callback_;
  std::unique_ptr<FIX::SocketInitiator> initiator_;
  std::unique_ptr<FIX::SessionSettings> settings_;
  std::unique_ptr<FIX::FileStoreFactory> storeFactory_;
  std::unique_ptr<FIX::FileLogFactory> logFactory_;
};

} // namespace ClearStreet
} // namespace QuanuX
