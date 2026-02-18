#pragma once

#ifdef USE_ONIXS

#include "../IEngine.hpp"
#include <OnixS/FixEngine.h>

namespace QuanuX::ClearStreet {

class OnixSBackend : public IEngine, public OnixS::FIX::ISessionListener {
public:
  OnixSBackend() : callback_(nullptr), session_(nullptr) {}
  ~OnixSBackend() { stop(); }

  void setCallback(IEngineCallback *callback) override { callback_ = callback; }

  bool init(const std::string &configFile) override {
    try {
      // OnixS Initialization
      OnixS::FIX::EngineSettings settings;
      settings.listenPort(1234); // Example, load from config
      OnixS::FIX::Engine::init(settings);

      // Create Session
      session_ = new OnixS::FIX::Session("SenderCompID", "TargetCompID",
                                         OnixS::FIX::FIX_4_2);
      session_->registerListener(this);
      return true;
    } catch (std::exception &e) {
      std::cerr << "OnixS Init Error: " << e.what() << std::endl;
      return false;
    }
  }

  void start() override {
    if (session_)
      session_->logonAsInitiator("host", 1234);
  }

  void stop() override {
    if (session_) {
      session_->logout();
      delete session_;
      session_ = nullptr;
    }
    OnixS::FIX::Engine::shutdown();
  }

  void sendOrder(const OrderRequest &req) override {
    // High Performance: Reuse FlatMessage or pre-allocated objects
    OnixS::FIX::Message msg;
    msg.set(OnixS::FIX::Tag::MsgType, OnixS::FIX::MsgType::NewOrderSingle);
    // ... set fields ...
    session_->send(&msg);
  }

  void cancelOrder(const std::string &orderID) override {
    // ...
  }

  // Performance Tuning
  void setAffinity(int sendCpu, int recvCpu) override {
    if (session_) {
      session_->setSendingThreadAffinity(sendCpu);
      session_->setReceivingThreadAffinity(recvCpu);
    }
  }

  void warmUp() override {
    // Pre-warm caches
    OnixS::FIX::Engine::warmUp();
  }

  // ISessionListener
  void onStateChange(OnixS::FIX::Session::State newState,
                     OnixS::FIX::Session::State oldState) override {
    if (newState == OnixS::FIX::Session::ACTIVE) {
      if (callback_)
        callback_->onLogon();
    } else if (newState == OnixS::FIX::Session::DISCONNECTED) {
      if (callback_)
        callback_->onLogout();
    }
  }

  void onInboundApplicationMsg(const OnixS::FIX::Message &msg,
                               OnixS::FIX::Session *sn) override {
    // Handle Execution Report
  }

private:
  IEngineCallback *callback_;
  OnixS::FIX::Session *session_;
};

} // namespace QuanuX::ClearStreet

#endif // USE_ONIXS
