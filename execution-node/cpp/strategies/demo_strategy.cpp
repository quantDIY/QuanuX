#include "quanux/indicators/market_profile.hpp"
#include "strategy_interface.h"
#include <iostream>
#include <memory>

// A dummy implementation of a self-contained strategy
class DemoStrategy {
private:
  // Uses the indicators library locally!
  quanux::indicators::MarketProfile<quanux::indicators::DenseStorage> profile_{
      100.0, 0.25};

public:
  void on_tick(double price) {
    // 1. Update Indicator locally (No RPC)
    profile_.process(price);

    // 2. Query Indicator
    int tpo = profile_.query(price);

    // 3. Make Decision (Logic)
    if (tpo > 10) {
      std::cout << "[DemoStrategy] High Traffic at " << price
                << ". Accu: " << tpo << "\n";
    }
  }
};

// Export C-style symbols if we were a dynamic plugin
extern "C" {
void run_demo_tick(double price) {
  static DemoStrategy strat;
  strat.on_tick(price);
}
}
