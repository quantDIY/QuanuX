#pragma once

#include <iostream>
#include <string>
// Assuming QuickFIX headers are in checking path or we ignore linter for now
#include "quickfix/Application.h"
#include "quickfix/FileLog.h"
#include "quickfix/FileStore.h"
#include "quickfix/MessageCracker.h"

namespace QuanuX {
namespace IBKR {

// Custom Tag Definitions
const int TAG_IBKR_LOCAL_SYMBOL = 6035;
const int TAG_ORDER_REF = 6010;

class Application : public FIX::Application, public FIX::MessageCracker {
public:
  Application();
  ~Application();

  // Application interface overrides
  void onCreate(const FIX::SessionID &) override;
  void onLogon(const FIX::SessionID &) override;
  void onLogout(const FIX::SessionID &) override;
  void toAdmin(FIX::Message &, const FIX::SessionID &) override;
  void toApp(FIX::Message &, const FIX::SessionID &) override;
  void fromAdmin(const FIX::Message &, const FIX::SessionID &) override;
  void fromApp(const FIX::Message &, const FIX::SessionID &) override;

  // Custom Methods
  void setAccount(const std::string &account);

private:
  std::string account_;
};

} // namespace IBKR
} // namespace QuanuX
  // namespace QuanuX::IBKR
