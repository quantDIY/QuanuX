#pragma once

#include "../IEngine.hpp"
#include <mutex>

namespace QuanuX::ClearStreet {

struct RiskConfig {
  double maxOrderQty = 100.0;
  double maxDailyLoss = 5000.0;
  double maxPositionSize = 1000.0;
};

class RiskEngine {
public:
  RiskEngine(const RiskConfig &config);

  // Returns true if order is safe, false if rejected
  bool checkOrder(const OrderRequest &order, std::string &rejectReason);

  void onFill(const ExecutionReport &fill);
  void reset();

private:
  RiskConfig config_;
  std::mutex mtx_;

  double currentDailyLoss_ = 0.0;
  double currentPosition_ = 0.0;
};

} // namespace QuanuX::ClearStreet
