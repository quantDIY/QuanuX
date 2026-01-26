#pragma once
#include "feed_interface.h"
#include <databento/live.hpp>
#include <databento/live_blocking.hpp>
#include <iostream>

class DatabentoFeed : public Feed {
public:
  DatabentoFeed(const std::string &key, const std::string &dataset,
                const std::string &symbol)
      : key_(key), dataset_(dataset), symbol_(symbol) {}

  void connect() override {
    // Build the client
    auto builder = databento::LiveBuilder().SetKey(key_).SetDataset(dataset_);

    client_ =
        std::make_unique<databento::LiveBlocking>(builder.BuildBlocking());

    std::cout << "[DatabentoFeed] Connected to " << dataset_ << std::endl;

    // Subscribe immediately
    std::vector<std::string> symbols = {symbol_};
    client_->Subscribe(symbols, databento::Schema::Trades,
                       databento::SType::RawSymbol);
    std::cout << "[DatabentoFeed] Subscribed to " << symbol_ << std::endl;
  }

  void start(std::function<void(const MarketUpdate &)> on_tick) override {
    if (!client_)
      throw std::runtime_error("Client not connected");

    client_->Start();
    std::cout << "[DatabentoFeed] Stream started." << std::endl;

    while (running_) {
      const databento::Record *rec =
          client_->NextRecord(std::chrono::seconds(1));
      if (!rec)
        continue; // Timeout, check running_

      if (rec->RType() == databento::RType::Mbp0) { // Trades
        auto *trade =
            reinterpret_cast<const databento::TradeMsg *>(&rec->Header());

        MarketUpdate update;
        update.timestamp = trade->ts_recv.time_since_epoch().count();
        update.instrument_id = trade->hd.instrument_id;
        update.price = trade->price * 1e-9;
        update.size = static_cast<double>(trade->size);
        update.is_trade = true;
        // Databento side: 'A'=Ask (Sell), 'B'=Bid (Buy)
        // MarketUpdate side: 1=Buy, -1=Sell
        update.side = (static_cast<char>(trade->side) == 'B') ? 1 : -1;

        on_tick(update);
      }
    }
  }

  void stop() override {
    running_ = false;
    if (client_)
      client_->Stop();
  }

private:
  std::string key_;
  std::string dataset_;
  std::string symbol_;
  std::unique_ptr<databento::LiveBlocking> client_;
  volatile bool running_ = true;
};
