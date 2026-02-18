#include "../src/Metrics/LatencyTracker.hpp"
#include "../src/Risk/RiskEngine.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace QuanuX::ClearStreet;

// Risk Engine Tests
TEST(RiskEngineTest, RejectsExcessiveOrderQty) {
  RiskConfig config;
  config.maxOrderQty = 100.0;
  RiskEngine risk(config);

  OrderRequest order;
  order.quantity = 200.0;
  std::string reason;

  EXPECT_FALSE(risk.checkOrder(order, reason));
  EXPECT_NE(reason.find("exceeds limit"), std::string::npos);
}

TEST(RiskEngineTest, AcceptsValidOrder) {
  RiskConfig config;
  config.maxOrderQty = 100.0;
  RiskEngine risk(config);

  OrderRequest order;
  order.quantity = 50.0;
  std::string reason;

  EXPECT_TRUE(risk.checkOrder(order, reason));
}

TEST(RiskEngineTest, TracksPositionLimits) {
  RiskConfig config;
  config.maxPositionSize = 100.0;
  RiskEngine risk(config);

  // Fill 1: +80 (Pos 80, OK)
  ExecutionReport fill1;
  fill1.side = "1"; // Buy
  fill1.lastQty = 80;
  risk.onFill(fill1);

  // Order 2: Buy 30 (Potential 110 > 100, Reject)
  OrderRequest order;
  order.side = "1";
  order.quantity = 30;
  std::string reason;

  EXPECT_FALSE(risk.checkOrder(order, reason));
}

// Latency Tracker Tests
TEST(LatencyTrackerTest, RecordsStats) {
  LatencyTracker tracker;
  tracker.record("test_tag", 100);
  tracker.record("test_tag", 200);
  tracker.record("test_tag", 50);

  std::string json = tracker.dump();
  // Simple substring check
  EXPECT_NE(json.find("\"test_tag\""), std::string::npos);
  EXPECT_NE(json.find("\"count\": 3"), std::string::npos);
  EXPECT_NE(json.find("\"min\": 50"), std::string::npos);
  EXPECT_NE(json.find("\"max\": 200"), std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
