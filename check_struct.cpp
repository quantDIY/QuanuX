#include "quanux/MarketTick.hpp"
#include <cstddef>
#include <iostream>

int main() {
  std::cout << "Sizeof MarketTick: " << sizeof(quanux::MarketTick) << std::endl;
  std::cout << "Alignof MarketTick: " << alignof(quanux::MarketTick)
            << std::endl;
  std::cout << "Offset internal_arrival_ts: "
            << offsetof(quanux::MarketTick, internal_arrival_ts) << std::endl;
  std::cout << "Offset _pad: " << offsetof(quanux::MarketTick, _pad)
            << std::endl;

  if (sizeof(quanux::MarketTick) == 64) {
    std::cout << "SUCCESS: 64 bytes" << std::endl;
    return 0;
  } else {
    std::cout << "FAILURE: " << sizeof(quanux::MarketTick) << " bytes"
              << std::endl;
    return 1;
  }
}
