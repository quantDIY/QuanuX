#pragma once

#include "quanux/omega/integration/annex_consumer_router.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include <iostream>

namespace quanux {
namespace omega {
namespace integration {

// A strictly read-only utility to filter and print payload boundaries.
// It subscribes (conceptually) to topics, parses headers, and dumps context
// without ever mutating or normalizing the payload further.
class AnnexDebugReader {
public:
    struct ReadFilters {
        bool show_lifecycle;
        bool show_corrections;
        bool show_invalid;
        std::string target_adapter; // If empty, show all
    };

    static void inspect_payload(
        std::string_view subject, 
        const AnnexTransportProjection& payload,
        const ReadFilters& filters) noexcept 
    {
        auto category = AnnexConsumerRouter::identify_category(subject);
        
        // 1. Filter logic
        if (category == AnnexConsumerRouter::SubjectCategory::LIFECYCLE && !filters.show_lifecycle) return;
        if (category == AnnexConsumerRouter::SubjectCategory::CORRECTIONS && !filters.show_corrections) return;
        if (category == AnnexConsumerRouter::SubjectCategory::INVALID && !filters.show_invalid) return;

        if (!filters.target_adapter.empty() && payload.adapter_name != filters.target_adapter) {
            return;
        }

        // 2. Read-only printing
        std::cout << "[AnnexDebugReader] Subject: " << subject << "\n";
        std::cout << "  Schema Version: " << payload.schema_version << "\n";
        std::cout << "  Event Type: " << static_cast<int>(payload.event_type) << "\n";
        std::cout << "  Event Identity: " << payload.event_id;
        if (!payload.firm_order_id.empty()) std::cout << " (Firm: " << payload.firm_order_id << ")";
        std::cout << "\n";

        // Specifically print provenance if present
        std::cout << "  Provenance | Adapter: " << payload.adapter_name << " (" << payload.adapter_version << ")\n";
        std::cout << "  Provenance | ParseStatus: " << static_cast<int>(payload.parse_status) << "\n";
        std::cout << "  Provenance | PayloadHash: " << payload.payload_hash << "\n";

        // If it's a correction, print linkage
        if (AnnexConsumerRouter::is_correction_event(payload.correction_type)) {
            std::cout << "  Linkage | Correction Type: " << static_cast<int>(payload.correction_type) << "\n";
        }

        // Specifically print native survival tags to help diagnose parser gaps
        if (!payload.venue_native_event_code.empty() || !payload.survival_tags.empty()) {
            std::cout << "  Survival | Venue Code: " << payload.venue_native_event_code << "\n";
            std::cout << "  Survival | Tags count: " << payload.survival_tags.size() << "\n";
        }
        std::cout << "------------------------------------------\n";
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
