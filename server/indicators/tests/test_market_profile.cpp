#include "quanux/indicators/market_profile.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace quanux::indicators;

void test_dense_profile() {
  std::cout << "[Test] Dense Profile... ";
  // Base 100, Tick 1.0
  MarketProfile<DenseStorage> profile(100.0, 1.0);

  profile.process(100.0); // Index 0
  profile.process(100.0); // Index 0
  profile.process(102.0); // Index 2

  assert(profile.query(100.0) == 2);
  assert(profile.query(101.0) == 0);
  assert(profile.query(102.0) == 1);

  std::cout << "PASSED\n";
}

void test_sparse_profile() {
  std::cout << "[Test] Sparse Profile... ";
  // Tick 0.25
  MarketProfile<SparseStorage> profile(0.25);

  profile.process(100.0);
  profile.process(100.0);
  profile.process(50000.0); // Far away

  assert(profile.query(100.0) == 2);
  assert(profile.query(50000.0) == 1);
  assert(profile.query(101.0) == 0);

  std::cout << "PASSED\n";
}

int main() {
  try {
    test_dense_profile();
    test_sparse_profile();
    std::cout << "ALL MARKET PROFILE TESTS PASSED\n";
  } catch (const std::exception &e) {
    std::cerr << "FAILED: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
