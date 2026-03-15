#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class EventType : uint8_t {
    Unknown = 0,
    OrderAcknowledged = 1,
    OrderRejected = 2,
    ExecutionPartial = 3,
    ExecutionFull = 4,
    ExecutionBust = 5,
    ExecutionCorrection = 6,
    CancelReplaced = 7,
    CancelRejected = 8,
    SessionGap = 9,
    ReplayRecovery = 10,
    AllocationMutated = 11
};

} // namespace vocab
} // namespace omega
} // namespace quanux
