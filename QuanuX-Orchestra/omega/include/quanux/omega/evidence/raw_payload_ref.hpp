#pragma once

#include <cstdint>
#include <cstddef>

namespace quanux {
namespace omega {
namespace evidence {

// Zero-copy reference to the raw venue payload that generated this event
struct RawPayloadRef {
    const uint8_t* data{nullptr};
    size_t size{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return data != nullptr && size > 0;
    }
};

} // namespace evidence
} // namespace omega
} // namespace quanux
