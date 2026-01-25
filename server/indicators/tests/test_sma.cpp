#include "quanux/indicators/sma.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

// Simple test runner
void test_lazy_sma() {
  std::cout << "[Test] Lazy SMA... ";
  std::vector<double> prices = {10, 20, 30, 40, 50};
  auto sma3 = prices | quanux::indicators::sma(3);

  // sma3 should be same size as input
  // 0: NaN
  // 1: NaN
  // 2: (10+20+30)/3 = 20
  // 3: (20+30+40)/3 = 30
  // 4: (30+40+50)/3 = 40

  auto it = sma3.begin();
  assert(std::isnan(*it));
  ++it;
  assert(std::isnan(*it));
  ++it;
  assert(std::abs(*it - 20.0) < 0.001);
  ++it;
  assert(std::abs(*it - 30.0) < 0.001);
  ++it;
  assert(std::abs(*it - 40.0) < 0.001);
  ++it;
  assert(it == sma3.end());

  std::cout << "PASSED\n";
}

void test_eager_sma() {
  std::cout << "[Test] Eager SMA... ";
  std::vector<double> prices = {10, 20, 30, 40, 50};
  auto result = quanux::indicators::compute_sma(prices, 3);

  assert(result.size() == 5);
  assert(std::isnan(result[0]));
  assert(std::isnan(result[1]));
  assert(std::abs(result[2] - 20.0) < 0.001);
  assert(std::abs(result[3] - 30.0) < 0.001);
  assert(std::abs(result[4] - 40.0) < 0.001);

  std::cout << "PASSED\n";
}

int main() {
  try {
    test_lazy_sma();
    test_eager_sma();
    std::cout << "ALL TESTS PASSED\n";
  } catch (const std::exception &e) {
    std::cerr << "FAILED: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
