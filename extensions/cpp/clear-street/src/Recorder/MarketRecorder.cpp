#include "MarketRecorder.hpp"
#include <iostream>

namespace QuanuX::ClearStreet {

MarketRecorder::MarketRecorder(const std::string &dbPath, RecorderMode mode)
    : mode_(mode) {
  // Initialize Archiver using simplified logic for now
  archiver_ = std::make_unique<DataArchiver>(dbPath);
  // Determine if L3 table is needed
  bool l3 = (mode == RecorderMode::L3_MARKET_BY_ORDER);
  archiver_->initSchema(l3);
}

MarketRecorder::~MarketRecorder() {}

void MarketRecorder::recordQuote(const std::string &symbol, double bid,
                                 double ask, int64_t timestamp) {
  // Always record Top of Book if available
  archiver_->appendQuote(symbol, bid, ask, timestamp);
}

void MarketRecorder::recordTrade(const std::string &symbol, double price,
                                 double size, int64_t timestamp) {
  // TODO: Add trade table in Archiver (omitted for brevity in scaffold)
}

void MarketRecorder::recordDepth(const std::string &symbol, int level,
                                 double price, double size, int side,
                                 int64_t timestamp) {
  if (mode_ == RecorderMode::L1_TOP_OF_BOOK)
    return;

  // Check if L2 or L3 mode allows depth recording
  // In L3 mode, we might still receive aggregated depth, but usually we prefer
  // orders. If L2 mode, definitely record.
  if (mode_ == RecorderMode::L2_MARKET_BY_PRICE) {
    archiver_->appendDepth(symbol, level, price, size, side, timestamp);
  }
}

void MarketRecorder::recordOrder(const std::string &symbol,
                                 const std::string &orderId, double price,
                                 double size, int side, int64_t timestamp) {
  if (mode_ != RecorderMode::L3_MARKET_BY_ORDER)
    return;

  archiver_->appendOrder(symbol, orderId, price, size, side, timestamp);
}

} // namespace QuanuX::ClearStreet
