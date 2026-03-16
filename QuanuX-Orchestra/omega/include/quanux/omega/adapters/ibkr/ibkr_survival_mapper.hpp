#pragma once

#include <span>
#include <string_view>
#include "quanux/omega/omega_core/omega_event_extensions.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_structs.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrSurvivalMapper {
public:
    static void map_extensions(
        const IbkrExecutionReportMock* msg, 
        core::OmegaEventExtensions& extensions,
        survival::TagValue& shared_tag) noexcept 
    {
        // 1. Venue native reason captures
        extensions.venue_native_reason_code = std::string_view(msg->whyHeld);
        
        // Treat status string as the native event code
        extensions.venue_native_event_code = std::string_view(msg->status);

        // 2. Unmapped IBKR protocol fields 
        // Example: The numeric error code acts as our single survivor tag here if it's > 0
        if (msg->errorCode > 0) {
            shared_tag.tag_id = 10000; // Arbitrary "Unmapped Code" tag id for this phase
            
            // Reinterpret bytes of errorCode directly for zero-copy survival
            shared_tag.value = std::string_view(
                reinterpret_cast<const char*>(&(msg->errorCode)), 
                sizeof(msg->errorCode)
            );
            
            extensions.tags = std::span<const survival::TagValue>(&shared_tag, 1);
        } else {
            extensions.tags = std::span<const survival::TagValue>(); // Empty
        }
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
