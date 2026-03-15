#pragma once

#include "quanux/omega/evidence/raw_payload_ref.hpp"
#include "quanux/omega/evidence/raw_payload_hash.hpp"
#include "quanux/omega/omega_vocab/parse_status.hpp"
#include <span>
#include <string_view>

namespace quanux {
namespace omega {
namespace core {

struct OmegaEventProvenance {
    evidence::RawPayloadRef raw_evidence;
    evidence::RawPayloadHash payload_hash;

    // Lifetimes: These views must point to static storage, interned strings, 
    // or string pools guaranteed to outlive this envelope view.
    std::string_view source_protocol;
    std::string_view adapter_name;
    std::string_view adapter_version;

    vocab::ParseStatus parse_status{vocab::ParseStatus::Unknown};

    // A list-oriented representation of parse warnings.
    // Lifetime: the backing array/buffer for this span must outlive the envelope view.
    std::span<const std::string_view> parse_warnings;
};

} // namespace core
} // namespace omega
} // namespace quanux
