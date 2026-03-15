#pragma once

#include "quanux/omega/survival/tag_value.hpp"
#include <span>
#include <string_view>

namespace quanux {
namespace omega {
namespace core {

// Zero-copy structure representing the true survival boundary for native venue mapping.
struct OmegaEventExtensions {
    // Lifetimes: Like all string_view fields here, these must point to buffers 
    // (like the RawPayloadRef payload) that strictly outlive the envelope view.
    std::string_view venue_native_event_code;
    std::string_view venue_native_reason_code;

    // The tags collection for preserving venue-specific mapping nuance.
    // Lifetime: The backing array for this span must be managed by parsing buffers
    // that outlive this envelope view.
    std::span<const survival::TagValue> tags;
};

} // namespace core
} // namespace omega
} // namespace quanux
