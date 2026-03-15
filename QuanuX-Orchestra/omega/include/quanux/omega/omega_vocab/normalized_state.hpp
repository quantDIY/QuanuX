#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class NormalizedState : uint8_t {
    Unknown = 0,
    PendingNew = 1,
    New = 2,
    PartiallyFilled = 3,
    Filled = 4,
    PendingCancel = 5,
    PendingReplace = 6,
    Canceled = 7,
    Replaced = 8,
    Rejected = 9,
    Suspended = 10,
    Expired = 11,
    Busted = 12
};

} // namespace vocab
} // namespace omega
} // namespace quanux
