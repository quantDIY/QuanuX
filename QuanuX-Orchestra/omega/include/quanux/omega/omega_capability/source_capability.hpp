#pragma once

#include <string_view>

namespace quanux {
namespace omega {
namespace capability {

// Versioning Groundwork for the external contract governance
struct SchemaVersion {
    std::string_view version_string;
    std::string_view compatibility_note;
    bool holds_deprecations;
};

// What the source can actually prove about timestamps natively
struct TimestampCapability {
    bool proves_source_native;
    bool proves_venue_gateway;
    bool proves_receive_nic;
};

// What the source can actually prove about linkage
struct LinkageCapability {
    bool proves_busts;
    bool proves_price_corrections;
    bool proves_quantity_corrections;
    bool maintains_deterministic_replay_state;
};

// A profile summarizing what an adapter specifically supports and proves
struct SourceCapabilityProfile {
    std::string_view adapter_name;
    SchemaVersion schema_compliance;
    TimestampCapability time_proofs;
    LinkageCapability linkage_proofs;
};

} // namespace capability
} // namespace omega
} // namespace quanux
