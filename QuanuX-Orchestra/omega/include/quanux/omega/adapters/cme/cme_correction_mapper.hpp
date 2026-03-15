#pragma once

#include "quanux/omega/omega_core/omega_event_linkage.hpp"
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeCorrectionMapper {
public:
    static void map_corrections(const CmeExecutionReportSbe* msg, core::OmegaEventLinkage& linkage) noexcept {
        // Flag linkage corrections if the CME message explicitly states it's a bust/correct.
        // In this mock, ord_status 5 == Bust, 6 == Correction.
        if (msg->ord_status == 5) {
            linkage.correction_type = core::CorrectionType::Bust;
        } else if (msg->ord_status == 6) {
            linkage.correction_type = core::CorrectionType::PriceCorrection;
        }
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
