#include "quanux/omega/translation/decoder_ring.hpp"
#include "quanux/omega/adapters/cme/cme_adapter.hpp"

namespace quanux {
namespace omega {
namespace translation {

bool DecoderRing::decode(
    VenueId venue_id, 
    const uint8_t* raw_buffer, 
    std::size_t buffer_len, 
    core::OmegaEventEnvelope& out_envelope,
    survival::TagValue& out_survival_tag) noexcept 
{
    // The registry routes to the zero-copy adapter based on the Venue identifier provided by the Annex's NATS feed.
    switch (venue_id) {
        case VenueId::CmeMdp3:
            return adapters::cme::CmeAdapter::parse_execution_report(
                raw_buffer, buffer_len, out_envelope, out_survival_tag);
        
        case VenueId::NasdaqOuch:
        case VenueId::IbkrOnixs:
        case VenueId::Unknown:
        default:
            return false; // Adapters not yet fully implemented
    }
}

} // namespace translation
} // namespace omega
} // namespace quanux
