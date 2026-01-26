#pragma once
#include "feed_interface.h"
#include <databento/dbn_file_store.hpp>
#include <databento/record.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class DbnFileFeed : public Feed {
public:
  explicit DbnFileFeed(const std::string &file_path) : file_path_(file_path) {}

  void connect() override {
    store_ = std::make_unique<databento::DbnFileStore>(file_path_);
    std::cout << "[DbnFileFeed] Loaded file: " << file_path_ << std::endl;
  }

  void start(std::function<void(const MarketUpdate &)> on_tick) override {
    if (!store_)
      throw std::runtime_error("Store not initialized");

    std::cout << "[DbnFileFeed] Replay started..." << std::endl;
    std::cout << "Debug: Invoking Replay..." << std::endl;

    store_->Replay([&](const databento::Record &rec) {
      std::cout << "Debug: Rec Type " << (int)rec.RType() << std::endl;
      if (!running_)
        return databento::KeepGoing::Stop;

      if (rec.RType() == databento::RType::Mbp0) { // Trades
        auto *trade =
            reinterpret_cast<const databento::TradeMsg *>(&rec.Header());

        MarketUpdate update;
        update.timestamp = trade->ts_recv.time_since_epoch().count();
        update.instrument_id = trade->hd.instrument_id;
        update.price = trade->price * 1e-9;
        update.size = static_cast<double>(trade->size);
        update.is_trade = true;
        update.side = (static_cast<char>(trade->side) == 'B') ? 1 : -1;

        on_tick(update);
      } else if (rec.RType() == databento::RType::Mbo) {
        // Support L3 records
        auto *mbo = reinterpret_cast<const databento::MboMsg *>(&rec.Header());

        // Interpreting MBO for strategy:
        // Action 'T' = Trade
        // Action 'A', 'C', 'M' = Quotes (Order Book updates)
        // For now, we will forward 'T' as trades, and others as "Quotes"
        // (though simplistic, it allows verifying data flow)

        bool is_trade = (mbo->action == 'T');

        MarketUpdate update;
        update.timestamp = mbo->ts_recv.time_since_epoch().count();
        update.instrument_id = mbo->hd.instrument_id;
        update.price = mbo->price * 1e-9;
        update.size = static_cast<double>(mbo->size);
        update.is_trade = is_trade;
        update.side = (static_cast<char>(mbo->side) == 'B') ? 1 : -1;

        on_tick(update);
      }

      return databento::KeepGoing::Continue;
    });

    std::cout << "[DbnFileFeed] Replay complete." << std::endl;
  }

  void stop() override { running_ = false; }

private:
  std::string file_path_;
  std::unique_ptr<databento::DbnFileStore> store_;
  bool running_ = true;
};
