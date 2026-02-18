#include "RiskEngine.hpp"
#include <cmath>

namespace QuanuX::ClearStreet {

RiskEngine::RiskEngine(const RiskConfig &config) : config_(config) {}

bool RiskEngine::checkOrder(const OrderRequest &order,
                            std::string &rejectReason) {
  std::lock_guard<std::mutex> lock(mtx_);

  // 1. Max Order Quantity
  if (order.quantity > config_.maxOrderQty) {
    rejectReason = "Risk: Order quantity " + std::to_string(order.quantity) +
                   " exceeds limit " + std::to_string(config_.maxOrderQty);
    return false;
  }

  // 2. Max Position Size (Approximate check before fill)
  double potentialPos =
      currentPosition_ + (order.side == "1" ? order.quantity : -order.quantity);
  if (std::abs(potentialPos) > config_.maxPositionSize) {
    rejectReason = "Risk: Potential position " + std::to_string(potentialPos) +
                   " exceeds limit " + std::to_string(config_.maxPositionSize);
    return false;
  }

  // 3. Daily Loss Limit
  if (currentDailyLoss_ > config_.maxDailyLoss) {
    rejectReason = "Risk: Daily loss limit triggered";
    return false;
  }

  return true;
}

void RiskEngine::onFill(const ExecutionReport &fill) {
  std::lock_guard<std::mutex> lock(mtx_);

  // Simplified PnL tracking for logic demonstration
  // Real PnL requires exhaustive match engine logic (FIFO/LIFO)
  if (fill.side == "1") {
    currentPosition_ += fill.lastQty;
  } else {
    currentPosition_ -= fill.lastQty;
  }

  // Note: Daily Loss calculation would require tracking avg entry price vs exit
  // price keeping it simple for the scaffold.
}

void RiskEngine::reset() {
  std::lock_guard<std::mutex> lock(mtx_);
  currentDailyLoss_ = 0;
  currentPosition_ = 0;
}

} // namespace QuanuX::ClearStreet
