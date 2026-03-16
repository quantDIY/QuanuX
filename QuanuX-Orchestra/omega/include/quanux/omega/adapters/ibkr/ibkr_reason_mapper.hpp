#pragma once

#include <string_view>
#include <cstdint>
#include "quanux/omega/omega_core/omega_event_semantics.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrReasonMapper {
public:
    static void map_reason(
        std::string_view status, 
        std::string_view why_held, 
        int32_t error_code, 
        core::OmegaEventSemantics& semantics) noexcept 
    {
        // 1. Defaults
        semantics.reason_code = vocab::NormalizedReasonCode::None;

        if (status == "Inactive" || status == "Cancelled") {
            // 2. Map strict numeric error codes if present
            switch (error_code) {
                case 110:
                    semantics.reason_code = vocab::NormalizedReasonCode::InvalidPrice;
                    return;
                case 135:
                    semantics.reason_code = vocab::NormalizedReasonCode::MarketClosed;
                    return;
                case 200:
                    semantics.reason_code = vocab::NormalizedReasonCode::InvalidSymbol;
                    return;
                case 399:
                    semantics.reason_code = vocab::NormalizedReasonCode::SessionDisconnected;
                    return;
                default:
                    break; 
            }

            // 3. Fallback to parsing the why_held string if the code was missing or generic 0
            if (!why_held.empty()) {
                if (why_held.find("margin") != std::string_view::npos) {
                    semantics.reason_code = vocab::NormalizedReasonCode::RiskLimitExceeded;
                } else if (why_held.find("duplicate") != std::string_view::npos) {
                    semantics.reason_code = vocab::NormalizedReasonCode::DuplicateOrder;
                } else if (why_held.find("too late") != std::string_view::npos) {
                    semantics.reason_code = vocab::NormalizedReasonCode::CancelTooLate;
                } else {
                    semantics.reason_code = vocab::NormalizedReasonCode::UnknownVenueReason;
                }
            } else if (error_code != 0) {
                // We have an unknown code, but no text reason
                semantics.reason_code = vocab::NormalizedReasonCode::UnknownVenueReason;
            }
        }
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
