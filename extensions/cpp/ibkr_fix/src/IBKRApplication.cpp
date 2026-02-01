#include "IBKRApplication.h"
#include "quickfix/Session.h"
#include <iostream>

namespace QuanuX {
namespace IBKR {

Application::Application() {}
Application::~Application() {}

void Application::onCreate(const FIX::SessionID &sessionID) {
  std::cout << "Session Created: " << sessionID << std::endl;
}

void Application::onLogon(const FIX::SessionID &sessionID) {
  std::cout << "Logon - IBKR Gateway Connected: " << sessionID << std::endl;
}

void Application::onLogout(const FIX::SessionID &sessionID) {
  std::cout << "Logout: " << sessionID << std::endl;
}

void Application::toAdmin(FIX::Message &message,
                          const FIX::SessionID &sessionID) {
  // IBKR requires Username/Password in Logon sometimes, but usually Gateway
  // handles auth. If we needed to inject credentials, we'd do it here.
}

void Application::toApp(FIX::Message &message,
                        const FIX::SessionID &sessionID) {
  // Inject Custom IBKR Tags into every outgoing order
  if (!account_.empty()) {
    message.setField(FIX::Account(account_));
  }

  // Example: Add time-in-force default if missing
  // if (!message.isSetField(FIX::FIELD::TimeInForce)) {
  //     message.setField(FIX::TimeInForce(FIX::TimeInForce_DAY));
  // }
}

void Application::fromAdmin(const FIX::Message &, const FIX::SessionID &) {}

void Application::fromApp(const FIX::Message &message,
                          const FIX::SessionID &sessionID) {
  try {
    crack(message, sessionID);
  } catch (FIX::DoNotSend &) {
  } catch (FIX::UnsupportedMessageType &) {
  }
}

void Application::setAccount(const std::string &account) { account_ = account; }

} // namespace IBKR
} // namespace QuanuX
  // namespace QuanuX::IBKR
