#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace ids {

struct EventId {
    uint64_t value{0};

    constexpr explicit EventId(uint64_t v) noexcept : value(v) {}
    constexpr EventId() noexcept = default;

    [[nodiscard]] constexpr bool is_valid() const noexcept { return value != 0; }
    constexpr bool operator==(const EventId& other) const noexcept { return value == other.value; }
    constexpr bool operator!=(const EventId& other) const noexcept { return value != other.value; }
};

} // namespace ids
} // namespace omega
} // namespace quanux
