#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class TimeInForce : uint8_t {
    Unknown = 0,
    Day = 1,
    GTC = 2,
    IOC = 3,
    FOK = 4,
    GTD = 5
};

} // namespace vocab
} // namespace omega
} // namespace quanux
