#pragma once
#include "RApiPlus.h"
#include "feed_interface.h"
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class RithmicFeed : public Feed, public RApi::RCallbacks {
public:
  RithmicFeed(const std::string &user, const std::string &pass,
              const std::string &symbol, const std::string &exchange,
              int start_time = 0, int end_time = 0)
      : user_(user), pass_(pass), symbol_(symbol), exchange_(exchange),
        start_time_(start_time), end_time_(end_time) {

    // Initialize Params
    RApi::REngineParams params;
    params.pAdmCallbacks = &logger_;

    // Explicitly set safe literals to avoid UAF
    params.sAppName.pData = (char *)"QuanuX";
    params.sAppName.iDataLen = 6;
    params.sAppVersion.pData = (char *)"1.0.0";
    params.sAppVersion.iDataLen = 5;

    // SSL & Environment Configuration

    // Log File
    params.sLogFilePath.pData = (char *)"./rithmic.log";
    params.sLogFilePath.iDataLen = 13;

    // 2. Setup ENVP
    env_strings_.push_back("MML_DMN_SRVR_ADDR=rituz00100.00.rithmic.com:65000~"
                           "rituz00100.00.rithmic.net:65000~rituz00100.00."
                           "theomne.net:65000~rituz00100.00.theomne.com:65000");
    env_strings_.push_back("MML_DOMAIN_NAME=rithmic_uat_dmz_domain");
    env_strings_.push_back("MML_LIC_SRVR_ADDR=rituz00100.00.rithmic.com:56000~"
                           "rituz00100.00.rithmic.net:56000~rituz00100.00."
                           "theomne.net:56000~rituz00100.00.theomne.com:56000");
    env_strings_.push_back("MML_LOC_BROK_ADDR=rituz00100.00.rithmic.com:64100");
    env_strings_.push_back("MML_LOGGER_ADDR=rituz00100.00.rithmic.com:45454~"
                           "rituz00100.00.rithmic.net:45454~rituz00100.00."
                           "theomne.net:45454~rituz00100.00.theomne.com:45454");
    env_strings_.push_back("MML_LOG_TYPE=log_net");

    // Absolute path to SSL certs
    env_strings_.push_back(
        "MML_SSL_CLNT_AUTH_FILE=/Users/Duncan/Antigravity/QuanuX/QuanuX/"
        "extensions/cpp/rithmic/sdk/etc/rithmic_ssl_cert_auth_params");

    env_strings_.push_back("USER=" + user);

    // Build char* array
    for (auto &s : env_strings_) {
      env_ptrs_.push_back(const_cast<char *>(s.data()));
    }
    env_ptrs_.push_back(nullptr); // Null terminator

    params.envp = env_ptrs_.data();
    params.pContext = nullptr;

    try {
      engine_ = new RApi::REngine(&params);
    } catch (OmneException &e) {
      std::cerr << "Rithmic Engine Init Failed: Error " << e.getErrorCode()
                << " (" << (e.getErrorString() ? e.getErrorString() : "Unknown")
                << ")" << std::endl;
      throw;
    }
  }

  ~RithmicFeed() {
    if (engine_)
      delete engine_;
  }

  void connect() override {
    std::cout << "[Rithmic] Connecting..." << std::endl;

    RApi::LoginParams loginParams;
    loginParams.pCallbacks = this; // Register callbacks

    // Market Data
    loginParams.sMdUser = to_tsNCharcb(user_);
    loginParams.sMdPassword = to_tsNCharcb(pass_);
    // Trading System
    loginParams.sTsUser = to_tsNCharcb(user_);
    loginParams.sTsPassword = to_tsNCharcb(pass_);
    // Historical Data
    loginParams.sIhUser = to_tsNCharcb(user_);
    loginParams.sIhPassword = to_tsNCharcb(pass_);
    // PnL
    loginParams.sPnlCnnctPt = to_tsNCharcb("login_agent_pnlc");

    // Connection Points (SSL/UAT)
    loginParams.sMdCnnctPt.pData = (char *)"login_agent_tpc";
    loginParams.sMdCnnctPt.iDataLen = 15;

    loginParams.sTsCnnctPt.pData = (char *)"login_agent_opc";
    loginParams.sTsCnnctPt.iDataLen = 15;

    loginParams.sIhCnnctPt.pData = (char *)"login_agent_historyc";
    loginParams.sIhCnnctPt.iDataLen = 20;

    int code = 0;
    if (!engine_->login(&loginParams, &code)) {
      throw std::runtime_error("Rithmic Login Failed Code: " +
                               std::to_string(code));
    }

    // Wait for login success via Alert callback
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return connected_.load(); });

    std::cout << "[Rithmic] Connected." << std::endl;
  }

  void start(std::function<void(const MarketUpdate &)> on_tick) override {
    on_tick_ = on_tick;

    std::cout << "[Rithmic] Subscribing to " << symbol_ << "..." << std::endl;

    int code = 0;
    tsNCharcb sExch = to_tsNCharcb(exchange_);
    tsNCharcb sTick = to_tsNCharcb(symbol_);

    int flags = RApi::MD_BEST | RApi::MD_PRINTS | RApi::MD_TRADE_VOLUME;

    if (start_time_ > 0) {
      std::cout << "[Rithmic] Replaying trades from " << start_time_ << " to "
                << end_time_ << "..." << std::endl;
      engine_->replayTrades(&sExch, &sTick, start_time_, end_time_, &code);
    } else {
      std::cout << "[Rithmic] Subscribing to Live Data..." << std::endl;
      engine_->subscribe(&sExch, &sTick, flags, &code);
    }

    running_ = true;
    while (running_) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  }

  void stop() override {
    running_ = false;
    int code = 0;
    engine_->logout(&code);
  }

  // --- RCallbacks Overrides ---

  virtual int Alert(RApi::AlertInfo *pInfo, void *pContext,
                    int *aiCode) override {
    if (pInfo->iAlertType == RApi::ALERT_LOGIN_COMPLETE) {
      std::lock_guard<std::mutex> lock(mutex_);
      connected_ = true;
      cv_.notify_all();
      std::cout << "[Rithmic] Alert: Login Complete." << std::endl;
    } else if (pInfo->iAlertType == RApi::ALERT_LOGIN_FAILED) {
      std::cout << "[Rithmic] Alert: Login Failed." << std::endl;
      std::lock_guard<std::mutex> lock(mutex_);
      connected_ = false;
      cv_.notify_all();
    }
    *aiCode = API_OK;
    return OK;
  }

  virtual int TradePrint(RApi::TradeInfo *pInfo, void *pContext,
                         int *aiCode) override {
    if (!on_tick_)
      return OK;

    MarketUpdate update;
    update.timestamp = 0;
    update.instrument_id = 0;
    update.price = pInfo->dPrice;
    update.size = (double)pInfo->llSize;
    update.is_trade = true;
    update.side = 0;

    on_tick_(update);

    *aiCode = API_OK;
    return OK;
  }

private:
  RApi::REngine *engine_ = nullptr;
  std::string user_;
  std::string pass_;
  std::string symbol_;
  std::string exchange_;
  int start_time_;
  int end_time_;

  // Keep alive for REngineParams
  std::vector<std::string> env_strings_;
  std::vector<char *> env_ptrs_;

  std::function<void(const MarketUpdate &)> on_tick_;
  std::atomic<bool> connected_{false};
  std::atomic<bool> running_{false};
  std::mutex mutex_;
  std::condition_variable cv_;

  // Logger stub
  class Logger : public RApi::AdmCallbacks {
    virtual int Alert(RApi::AlertInfo *pInfo, void *pContext,
                      int *aiCode) override {
      return OK;
    }
  } logger_;

  // Helper
  tsNCharcb to_tsNCharcb(const std::string &s) {
    tsNCharcb t;
    t.pData = const_cast<char *>(s.data());
    t.iDataLen = (int)s.size();
    return t;
  }
};
