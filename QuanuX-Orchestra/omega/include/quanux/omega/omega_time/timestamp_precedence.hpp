#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace time {

enum class TimestampPrecedence : uint8_t {
    SourceNative = 1,   // Timestamp matching the venue's native generating clock
    SourceGateway = 2,  // Timestamp assigned at venue API gateway 
    CaptureIngress = 3, // Timestamp assigned at network capture ingress
    AdapterProcess = 4, // Timestamp assigned by adapter upon parsing
    OmegaNormalize = 5  // Timestamp assigned during normalization completion
};

struct TimestampNode {
    uint64_t epoch_nanos{0};
    TimestampPrecedence precedence{TimestampPrecedence::OmegaNormalize};

    [[nodiscard]] constexpr bool is_valid() const noexcept { return epoch_nanos != 0; }
};

} // namespace time
} // namespace omega
} // namespace quanux
