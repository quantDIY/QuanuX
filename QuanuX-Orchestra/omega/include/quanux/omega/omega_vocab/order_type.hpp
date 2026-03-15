#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class OrderType : uint8_t {
    Unknown = 0,
    Market = 1,
    Limit = 2,
    Stop = 3,
    StopLimit = 4
};

} // namespace vocab
} // namespace omega
} // namespace quanux
