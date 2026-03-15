#pragma once

#include <cstdint>
#include <string_view>

namespace quanux {
namespace omega {
namespace survival {

// Nuance preservation for unmapped/native venue tags
struct TagValue {
    uint32_t tag_id{0}; // e.g FIX tag or custom key
    std::string_view value;

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return !value.empty();
    }
};

} // namespace survival
} // namespace omega
} // namespace quanux
