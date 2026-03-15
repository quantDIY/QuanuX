#pragma once

#include "quanux/omega/omega_core/omega_event_semantics.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeReasonMapper {
public:
    static void map_reason(uint8_t cme_ord_status, uint16_t cme_error_code, core::OmegaEventSemantics& semantics) noexcept {
        // If it's a rejection, map the error code
        if (cme_ord_status == 8) { // Rejected
            switch (cme_error_code) {
                case 100:
                    semantics.reason_code = vocab::NormalizedReasonCode::InvalidSymbol;
                    break;
                case 101:
                    semantics.reason_code = vocab::NormalizedReasonCode::InvalidPrice;
                    break;
                default:
                    semantics.reason_code = vocab::NormalizedReasonCode::UnknownVenueReason;
                    break;
            }
        } else {
            semantics.reason_code = vocab::NormalizedReasonCode::None;
        }
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
