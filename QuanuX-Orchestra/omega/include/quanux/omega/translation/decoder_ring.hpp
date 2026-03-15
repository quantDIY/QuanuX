#pragma once

#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/survival/tag_value.hpp"

namespace quanux {
namespace omega {
namespace translation {

enum class VenueId : uint16_t {
    Unknown = 0,
    CmeMdp3 = 1,
    NasdaqOuch = 2,
    IbkrOnixs = 3
};

// Stateless Decoder Ring. Post-execution Annex provides a venue ID and a raw buffer pointer.
// The registry yields a populated canonical Event Envelope.
class DecoderRing {
public:
    // Decodes the raw payload into the provided envelope and survival tag.
    // Returns true if successfully recognized and mapped, false otherwise.
    static bool decode(
        VenueId venue_id, 
        const uint8_t* raw_buffer, 
        std::size_t buffer_len, 
        core::OmegaEventEnvelope& out_envelope,
        survival::TagValue& out_survival_tag) noexcept;
};

} // namespace translation
} // namespace omega
} // namespace quanux
