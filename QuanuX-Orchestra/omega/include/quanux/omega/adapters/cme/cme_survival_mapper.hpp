#pragma once

#include <string_view>
#include "quanux/omega/omega_core/omega_event_extensions.hpp"
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeSurvivalMapper {
public:
    static void map_survival_fields(
        const CmeExecutionReportSbe* msg, 
        core::OmegaEventExtensions& extensions, 
        survival::TagValue& out_survival_tag) noexcept 
    {
        // 1. Explicit Native Codes
        // Assuming template_id is used as a stand-in for native event code here
        extensions.venue_native_event_code = std::string_view(
            reinterpret_cast<const char*>(&msg->template_id), 
            sizeof(msg->template_id)
        );

        // 2. Tag Survival (Unmapped native codes)
        if (msg->unmapped_native_code != 0) {
            out_survival_tag.tag_id = 9999; // Arbitrary custom Tag ID for demo
            out_survival_tag.value = std::string_view(
                reinterpret_cast<const char*>(&msg->unmapped_native_code), 
                sizeof(msg->unmapped_native_code)
            );
            
            // Assign the bound span back to the envelope extensions view
            extensions.tags = std::span<const survival::TagValue>(&out_survival_tag, 1);
        } else {
            extensions.tags = std::span<const survival::TagValue>();
        }
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
