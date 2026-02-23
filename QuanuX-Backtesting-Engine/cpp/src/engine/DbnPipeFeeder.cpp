#include "engine/DbnPipeFeeder.h"
#include "engine/NatsReplayer.h"
#include <databento/dbn.hpp>
#include <databento/dbn_decoder.hpp>
#include <databento/ireadable.hpp> // Ensure this matches include path
#include <databento/record.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unistd.h> // for read, STDIN_FILENO

namespace quanux::engine {

class StdinReadable : public databento::IReadable {
public:
  void ReadExact(std::uint8_t *buffer, std::size_t length) override {
    std::size_t total_read = 0;
    while (total_read < length) {
      ssize_t res =
          ::read(STDIN_FILENO, buffer + total_read, length - total_read);
      if (res < 0) {
        throw std::runtime_error("StdinReadable::ReadExact failed");
      }
      if (res == 0) {
        throw std::runtime_error("StdinReadable::ReadExact unexpected EOF");
      }
      total_read += res;
    }
  }

  std::size_t ReadSome(std::uint8_t *buffer, std::size_t max_length) override {
    ssize_t res = ::read(STDIN_FILENO, buffer, max_length);
    if (res < 0) {
      throw std::runtime_error("StdinReadable::ReadSome failed");
    }
    return static_cast<std::size_t>(res);
  }
};

DbnPipeFeeder::DbnPipeFeeder(SimulatedExchange *exchange,
                             NatsReplayer *replayer)
    : exchange_(exchange), replayer_(replayer) {
  // Transfer ownership of StdinReadable to DbnDecoder
  decoder_ = std::make_unique<databento::DbnDecoder>(
      std::make_unique<StdinReadable>());
}

DbnPipeFeeder::~DbnPipeFeeder() {}

void DbnPipeFeeder::run() {
  std::cout << "[DbnPipeFeeder] Waiting for DBN data on stdin..." << std::endl;

  while (true) {
    // DbnDecoder pulls from StdinAdapter automatically
    const databento::Record *record = decoder_->DecodeRecord();
    if (!record) {
      break; // EOF
    }

    if (record->RType() == databento::RType::Mbo) {
      const auto *mbo =
          reinterpret_cast<const databento::MboMsg *>(&record->Header());

      bool is_bid = (mbo->side == databento::Side::Bid);
      // bool is_add = (mbo->action == databento::Action::Add);

      if (mbo->action == databento::Action::Add) {
        exchange_->on_market_data(mbo->order_id, mbo->price, mbo->size, is_bid,
                                  true);
        if (replayer_) {
          // Re-broadcast add (TODO: Use proper symbol from metadata)
          replayer_->publish_tick(
              "SIM", mbo->hd.ts_event.time_since_epoch().count(),
              (double)mbo->price / 1000000000.0, mbo->size, is_bid);
        }
      } else if (mbo->action == databento::Action::Cancel) {
        exchange_->on_market_data(mbo->order_id, 0, 0, is_bid, false);
      } else if (mbo->action == databento::Action::Trade) {
        exchange_->on_market_trade(mbo->price, mbo->size, is_bid);
        if (replayer_) {
          // Trades are crucial for live simulations
          replayer_->publish_tick(
              "SIM", mbo->hd.ts_event.time_since_epoch().count(),
              (double)mbo->price / 1000000000.0, mbo->size, is_bid);
        }
      }
    }
  }
  std::cout << "[DbnPipeFeeder] EOF reached." << std::endl;
}

} // namespace quanux::engine
