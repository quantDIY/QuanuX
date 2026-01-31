#pragma once

// This Shim is compiled ONLY when the OnixS SDK is injected.
// It maps QuanuX's generic Execution Interface to OnixS's High-Performance FIX Engine.

#include <QuanuX/StrategyInterface.hpp>
#include <OnixS/FixEngine.h> // This path is valid ONLY after 'quanuxctl integrate'

namespace QuanuX::OnixS {

    class OnixSAdapter : public QuanuX::IExecutionProvider {
    public:
        OnixSAdapter(const std::string& configFile) {
            // Initialize OnixS Engine
            // Note: This proprietary code is never committed. It exists only on the user's machine.
            OnixS::FIX::Engine::init(configFile);
        }

        virtual void sendOrder(const Order& order) override {
            // Map QuanuX Order -> OnixS FIX Message
            OnixS::FIX::Message fixMsg;
            // ... implementation details ...
            session_->send(&fixMsg);
        }

    private:
        OnixS::FIX::Session* session_;
    };

}
