#pragma once

#include <string>
#include <string_view>
#include "quanux/omega/omega_capability/schema_versioning.hpp"
#include "quanux/omega/integration/annex_consumer_router.hpp"

namespace quanux {
namespace omega {
namespace integration {

// Payload projection making downstream clients explicitly aware of schema bounds and numeric warnings
struct AnnexSchemaAnnouncement {
    std::string current_schema_version;
    std::string minimum_accepted_version;
    std::string deprecation_warning;
};

class AnnexSchemaPublisher {
public:
    static AnnexSchemaAnnouncement project_schema() noexcept {
        AnnexSchemaAnnouncement announcement;
        
        announcement.current_schema_version = "v" + 
            std::to_string(capability::SchemaVersioning::CURRENT_OMEGA_SCHEMA_VERSION.major) + "." +
            std::to_string(capability::SchemaVersioning::CURRENT_OMEGA_SCHEMA_VERSION.minor) + "." +
            std::to_string(capability::SchemaVersioning::CURRENT_OMEGA_SCHEMA_VERSION.patch);
            
        announcement.minimum_accepted_version = "v" + 
            std::to_string(capability::SchemaVersioning::MINIMUM_ACCEPTED_SCHEMA_VERSION.major) + "." +
            std::to_string(capability::SchemaVersioning::MINIMUM_ACCEPTED_SCHEMA_VERSION.minor) + "." +
            std::to_string(capability::SchemaVersioning::MINIMUM_ACCEPTED_SCHEMA_VERSION.patch);
            
        // Embed the critical numeric displacement warning on the wire
        announcement.deprecation_warning = std::string(capability::SchemaVersioning::PROVISIONAL_NUMERICS_DEPRECATION_WARNING);
        
        return announcement;
    }
};

class AnnexSchemaConsumer {
public:
    struct ConsumeResult {
        bool consumed;
        std::string rejection_reason;
        bool has_active_deprecation;
    };

    static ConsumeResult consume_announcement(
        std::string_view subject, 
        const AnnexSchemaAnnouncement& announcement) noexcept 
    {
        auto category = AnnexConsumerRouter::identify_category(subject);
        if (category != AnnexConsumerRouter::SubjectCategory::SCHEMA) {
            return {false, "Subject taxonomy violation: schema announcement not on schema stream", false};
        }

        if (announcement.current_schema_version.empty() || announcement.minimum_accepted_version.empty()) {
            return {false, "Missing required schema version bounds", false};
        }

        bool has_deprecation = !announcement.deprecation_warning.empty();

        return {true, "Consumed Schema successfully", has_deprecation};
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
