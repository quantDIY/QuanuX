#pragma once

#include "../Data/DataArchiver.hpp"
#include <memory>
#include <string>
#include <vector>

namespace QuanuX::ClearStreet {

enum class RecorderMode {
  L1_TOP_OF_BOOK,
  L2_MARKET_BY_PRICE,
  L3_MARKET_BY_ORDER
};

class MarketRecorder {
public:
  MarketRecorder(const std::string &dbPath, RecorderMode mode);
  ~MarketRecorder();

  void recordQuote(const std::string &symbol, double bid, double ask,
                   int64_t timestamp);
  void recordTrade(const std::string &symbol, double price, double size,
                   int64_t timestamp);
  void recordDepth(const std::string &symbol, int level, double price,
                   double size, int side, int64_t timestamp);
  void recordOrder(const std::string &symbol, const std::string &orderId,
                   double price, double size, int side, int64_t timestamp);

private:
  std::unique_ptr<DataArchiver> archiver_;
  RecorderMode mode_;
};

} // namespace QuanuX::ClearStreet
