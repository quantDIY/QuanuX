#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class SourceType : uint8_t {
    Unknown = 0,
    ExchangeDirect = 1,
    BrokerAlgorithm = 2,
    InternalCrossing = 3,
    DropCopy = 4,
    ManualEntry = 5,
    Simulated = 6
};

} // namespace vocab
} // namespace omega
} // namespace quanux
