#pragma once

#include "quanux/omega/survival/tag_value.hpp"
#include <span>
#include <string>
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

    // Optional backing arena for adapters that need to synthesize strings
    // (e.g., formatting an integer error code into a string) that do not natively
    // reside in the zero-copy ingress buffer.
    std::string _backing_native_event_code;
    std::string _backing_native_reason_code;
    std::string _backing_opaque_extension_blob;
    
    // The tags collection for preserving venue-specific mapping nuance.
    // Lifetime: The backing array for this span must be managed by parsing buffers
    // that outlive this envelope view.
    std::span<const survival::TagValue> tags;
    
    // Small inline buffer for tags synthesized by the adapter during parse
    // to avoid heap allocations or unsafe statics.
    survival::TagValue _inline_tags[4];
    size_t _inline_tag_count{0};

    // Helper to safely assign synthesized tags without static buffers
    void emplace_inline_tag(const survival::TagValue& tag) {
        if (_inline_tag_count < 4) {
            _inline_tags[_inline_tag_count++] = tag;
            tags = std::span<const survival::TagValue>(_inline_tags, _inline_tag_count);
        }
    }
};

} // namespace core
} // namespace omega
} // namespace quanux
