#include "quanux/indicators/volume_profile.hpp"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>

using namespace quanux::indicators;

void test_dense_vol_profile() {
  std::cout << "[Test] Dense Volume Profile... ";
  // Base 100, Tick 1.0
  VolumeProfile<DenseStorage> profile(100.0, 1.0);

  profile.process(100.0, 500.0);
  profile.process(100.0, 200.0);
  profile.process(102.0, 1000.0);

  assert(std::abs(profile.query(100.0) - 700.0) < 0.001);
  assert(std::abs(profile.query(101.0) - 0.0) < 0.001);
  assert(std::abs(profile.query(102.0) - 1000.0) < 0.001);

  std::cout << "PASSED\n";
}

void test_sparse_vol_profile() {
  std::cout << "[Test] Sparse Volume Profile... ";
  // Tick 0.25
  VolumeProfile<SparseStorage> profile(0.25);

  profile.process(100.0, 1.5);
  profile.process(100.0, 2.5);

  // Test accumulation
  assert(std::abs(profile.query(100.0) - 4.0) < 0.001);

  std::cout << "PASSED\n";
}

int main() {
  try {
    test_dense_vol_profile();
    test_sparse_vol_profile();
    std::cout << "ALL VOLUME PROFILE TESTS PASSED\n";
  } catch (const std::exception &e) {
    std::cerr << "FAILED: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
