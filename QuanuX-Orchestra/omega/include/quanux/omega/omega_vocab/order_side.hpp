#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class OrderSide : uint8_t {
    Unknown = 0,
    Buy = 1,
    Sell = 2
};

} // namespace vocab
} // namespace omega
} // namespace quanux
