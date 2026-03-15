#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace vocab {

enum class NormalizedReasonCode : uint16_t {
    None = 0,
    RiskLimitExceeded = 1,
    InvalidPrice = 2,
    InvalidSize = 3,
    InvalidSymbol = 4,
    MarketClosed = 5,
    SessionDisconnected = 6,
    CancelTooLate = 7,
    DuplicateOrder = 8,
    VenueTechnicalFault = 9,
    UnknownVenueReason = 999
};

} // namespace vocab
} // namespace omega
} // namespace quanux
