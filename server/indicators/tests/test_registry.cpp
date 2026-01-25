#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

// This include should work because of the target_include_directories change
#include "community_test/community_rsi.hpp"

void test_community_indicator() {
  std::cout << "[Test] Community Registry... ";
  std::vector<double> prices = {10, 20, 30, 40, 50};

  // Usage syntax: prices | rsi(14)
  auto rsi_view = prices | community::test::rsi(14);

  for (auto val : rsi_view) {
    assert(std::abs(val - 50.0) < 0.001); // Our dummy always returns 50.0
  }

  std::cout << "PASSED\n";
}

int main() {
  try {
    test_community_indicator();
    std::cout << "ALL REGISTRY TESTS PASSED\n";
  } catch (const std::exception &e) {
    std::cerr << "FAILED: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
