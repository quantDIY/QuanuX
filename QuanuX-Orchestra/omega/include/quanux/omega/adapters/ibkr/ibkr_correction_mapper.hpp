#pragma once

#include <string_view>
#include "quanux/omega/omega_core/omega_event_linkage.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_structs.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrCorrectionMapper {
public:
    static void map_linkage(const IbkrExecutionReportMock* msg, core::OmegaEventLinkage& linkage) noexcept {
        // Evaluate correction logic. In IBKR, typically we get explicit actions or states for cancel/replace.
        // We evaluate based on the mock status.
        std::string_view status(msg->status);
        
        if (status == "Bust" || status == "TradeCancelled") {
            linkage.correction_type = core::CorrectionType::Bust;
        } else if (status == "PriceChanged" || status == "SizeChanged") {
            linkage.correction_type = core::CorrectionType::PriceCorrection; // Simplified for the bridge
        } else {
            // Not a correction
            linkage.correction_type = core::CorrectionType::None;
        }
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
