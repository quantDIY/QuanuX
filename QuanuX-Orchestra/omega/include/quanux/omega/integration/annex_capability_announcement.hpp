#pragma once

#include <string>
#include <string_view>
#include "quanux/omega/omega_capability/source_capability.hpp"
#include "quanux/omega/integration/annex_consumer_router.hpp"

namespace quanux {
namespace omega {
namespace integration {

// Payload projection representing the formal capability proofs of an adapter 
// being announced to the wider network.
struct AnnexCapabilityAnnouncement {
    std::string adapter_name;
    std::string schema_compliance_version;    
    
    // Time Proofs
    bool proves_source_native_time;
    bool proves_venue_gateway_time;
    bool proves_receive_nic_time;

    // Linkage Proofs
    bool proves_busts;
    bool proves_price_corrections;
    bool proves_quantity_corrections;
    bool maintains_deterministic_replay_state;
    
    // Safety warnings explicitly propagated
    std::string capability_warning;
};

class AnnexCapabilityPublisher {
public:
    static AnnexCapabilityAnnouncement project_capability(
        const capability::SourceCapabilityProfile& profile) noexcept 
    {
        AnnexCapabilityAnnouncement announcement;
        announcement.adapter_name = std::string(profile.adapter_name);
        announcement.schema_compliance_version = std::string(profile.schema_compliance.version_string);

        // Propagate the numeric deprecation or other warnings
        if (profile.schema_compliance.holds_deprecations) {
             announcement.capability_warning = std::string(profile.schema_compliance.compatibility_note);
        }

        announcement.proves_source_native_time = profile.time_proofs.proves_source_native;
        announcement.proves_venue_gateway_time = profile.time_proofs.proves_venue_gateway;
        announcement.proves_receive_nic_time = profile.time_proofs.proves_receive_nic;
        
        announcement.proves_busts = profile.linkage_proofs.proves_busts;
        announcement.proves_price_corrections = profile.linkage_proofs.proves_price_corrections;
        announcement.proves_quantity_corrections = profile.linkage_proofs.proves_quantity_corrections;
        announcement.maintains_deterministic_replay_state = profile.linkage_proofs.maintains_deterministic_replay_state;

        return announcement;
    }
};

class AnnexCapabilityConsumer {
public:
    struct ConsumeResult {
        bool consumed;
        std::string rejection_reason;
    };

    static ConsumeResult consume_announcement(
        std::string_view subject, 
        const AnnexCapabilityAnnouncement& announcement) noexcept 
    {
        auto category = AnnexConsumerRouter::identify_category(subject);
        if (category != AnnexConsumerRouter::SubjectCategory::CAPABILITY) {
            return {false, "Subject taxonomy violation: capability message not on capability stream"};
        }

        if (announcement.adapter_name.empty()) {
            return {false, "Missing required field: adapter_name"};
        }

        // Capabilities must not overclaim source logic
        if (announcement.adapter_name == "IBKR_TWS_API" && announcement.proves_source_native_time) {
            return {false, "Overclaim Violation: Text APIs cannot prove native logic-gate timestamps"};
        }

        return {true, "Consumed Capability Successfully"};
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
