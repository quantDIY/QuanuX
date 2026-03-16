#pragma once

#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/omega_capability/schema_versioning.hpp"
#include <iostream>
#include <string_view>

namespace quanux {
namespace omega {
namespace integration {

class AnnexConsumerValidator {
public:
    // Result of validation - determines if payload can proceed safely.
    struct ValidationResult {
        bool is_valid;
        std::string rejection_reason;
        bool is_provisional_numerics_warning_active;
    };

    static ValidationResult validate_payload(const AnnexTransportProjection& payload) noexcept {
        ValidationResult result{true, "", false};

        // 1. Validate Schema Form and Content
        // Parse the payload schema string (e.g. "v1.0.0") to struct
        int major = 0, minor = 0, patch = 0;
        if (sscanf(payload.schema_version.c_str(), "v%d.%d.%d", &major, &minor, &patch) != 3) {
            return {false, "Could not parse schema version string: " + payload.schema_version, false};
        }
        
        capability::OmegaSchemaVersion parsed_version{major, minor, patch};
        
        auto compat_status = capability::SchemaVersioning::check_schema_compatibility(parsed_version);
        if (compat_status == capability::SchemaCompatibilityStatus::REJECTED_OUTRIGHT) {
            return {false, "Schema version rejected outright: " + payload.schema_version, false};
        }
        
        if (compat_status == capability::SchemaCompatibilityStatus::DEPRECATED_BUT_ACCEPTED) {
            result.is_provisional_numerics_warning_active = true;
        }

        // 2. Required Canonical Fields validation based on omega_schema.graphql specifications.
        // Even dead letters must have identifying provenance bounds, and lifecycle events must have an event_id.
        if (payload.adapter_name.empty()) {
             return {false, "Missing required provenance identity: adapter_name", false};
        }
        
        if (payload.payload_hash.empty()) {
             return {false, "Missing required provenance linkage: payload_hash", false};
        }

        // If it's not a parser error, it must have core semantic boundary identifiers
        if (payload.parse_status != vocab::ParseStatus::Error) {
             if (payload.event_id.empty() || payload.event_id == "0") {
                 return {false, "Valid normalized payloads must present an event_id", false};
             }
        }

        return result;
    }

    // Helper to extract numeric warnings explicitly on the consumer boundary
    static void print_schema_warnings(const ValidationResult& result) {
        if (result.is_provisional_numerics_warning_active) {
            std::cerr << capability::SchemaVersioning::PROVISIONAL_NUMERICS_DEPRECATION_WARNING << std::endl;
        }
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
