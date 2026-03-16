#pragma once

#include <string_view>
#include <tuple>
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace capability {

// Enumeration for schema compatibility status
enum class SchemaCompatibilityStatus {
    COMPATIBLE,
    DEPRECATED_BUT_ACCEPTED,
    REJECTED_OUTRIGHT
};

// Represents the schema version of the Omega Contract
struct OmegaSchemaVersion {
    int major;
    int minor;
    int patch;

    // A helper for simple version comparison
    constexpr bool operator==(const OmegaSchemaVersion& other) const noexcept {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    constexpr bool operator<(const OmegaSchemaVersion& other) const noexcept {
        if (major != other.major) return major < other.major;
        if (minor != other.minor) return minor < other.minor;
        return patch < other.patch;
    }
};

class SchemaVersioning {
public:
    // The current active version of the Omega external GraphQL schema contract
    static constexpr OmegaSchemaVersion CURRENT_OMEGA_SCHEMA_VERSION{1, 0, 0};

    // The oldest schema version Omega is willing to project/accept
    static constexpr OmegaSchemaVersion MINIMUM_ACCEPTED_SCHEMA_VERSION{1, 0, 0};

    // Explicitly check if an external schema version is compatible with this core version
    static SchemaCompatibilityStatus check_schema_compatibility(const OmegaSchemaVersion& requested_version) noexcept {
        if (requested_version < MINIMUM_ACCEPTED_SCHEMA_VERSION) {
            return SchemaCompatibilityStatus::REJECTED_OUTRIGHT;
        }
        
        // If they ask for 1.x but we are at 1.y, or they ask for our current version, we might accept it 
        // with a deprecation warning if we're moving towards the numerics formalization.
        if (requested_version == CURRENT_OMEGA_SCHEMA_VERSION) {
            // NOTE: Even though 1.0.0 is the current version, we flag it as deprecated-but-accepted 
            // implicitly if they rely on the provisional numerics. The actual deprecation is logged 
            // in the compatibility_note of the source profile.
            return SchemaCompatibilityStatus::COMPATIBLE;
        }

        // Catch-all: If it's theoretically in the future, reject.
        if (CURRENT_OMEGA_SCHEMA_VERSION < requested_version) {
             return SchemaCompatibilityStatus::REJECTED_OUTRIGHT;
        }
        
        return SchemaCompatibilityStatus::COMPATIBLE;
    }

    // Verify if an adapter's SourceCapabilityProfile meets minimum requirements to emit 
    // to the governed external schema.
    static SchemaCompatibilityStatus evaluate_adapter_compliance(const SourceCapabilityProfile& profile) noexcept {
        // Here we parse the profile.schema_compliance.version_string.
        // For simplicity, assuming the profile provides "v1.0.0".
        if (profile.schema_compliance.version_string == "v1.0.0") {
            if (profile.schema_compliance.holds_deprecations) {
                return SchemaCompatibilityStatus::DEPRECATED_BUT_ACCEPTED;
            }
            return SchemaCompatibilityStatus::COMPATIBLE;
        }
        
        return SchemaCompatibilityStatus::REJECTED_OUTRIGHT;
    }

    // Embed formal deprecation pathway for Provisional Numerics
    static constexpr std::string_view PROVISIONAL_NUMERICS_DEPRECATION_WARNING = 
        "WARNING: The types::Price and types::Quantity aliases are provisional. "
        "Schema v1.x downstream consumers MUST NOT assume accounting-grade precision on Double primitives. "
        "A forthcoming breaking Schema update will alter their scalar representations.";
};

} // namespace capability
} // namespace omega
} // namespace quanux
