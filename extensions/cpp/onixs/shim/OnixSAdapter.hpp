#pragma once

// This Shim is compiled ONLY when the OnixS SDK is injected.
// It maps QuanuX's generic Execution Interface to OnixS's High-Performance FIX
// Engine.

#include "../../common/include/QuanuX/Common.hpp"
#if __has_include(<OnixS/FixEngine.h>)
#include <OnixS/FixEngine.h>
#else
// Mock OnixS SDK for development/linting when SDK is not present
#include <string>
namespace OnixS {
namespace FIX {
class Engine {
public:
  static void init(const std::string &) {}
};
class Message {};
class Session {
public:
  void send(Message *) {}
};
} // namespace FIX
} // namespace OnixS
#endif

namespace QuanuX::OnixS {

class OnixSAdapter : public QuanuX::IExecutionProvider {
public:
  OnixSAdapter(const std::string &configFile) {
    // Initialize OnixS Engine
    // Note: This proprietary code is never committed. It exists only on the
    // user's machine.
    ::OnixS::FIX::Engine::init(configFile);
  }

  virtual void sendOrder(const Order &order) override {
    // Map QuanuX Order -> OnixS FIX Message
    ::OnixS::FIX::Message fixMsg;
    // ... implementation details ...
    session_->send(&fixMsg);
  }

  virtual bool connect() override {
    // In a real implementation, this would establish the FIX session
    if (session_)
      return true;
    return false;
  }

private:
  ::OnixS::FIX::Session *session_;
};

} // namespace QuanuX::OnixS
