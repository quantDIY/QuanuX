#pragma once

#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer_router.hpp"
#include "quanux/omega/integration/annex_consumer_validator.hpp"
#include <iostream>

namespace quanux {
namespace omega {
namespace integration {

// Consumer result encapsulating strictly governed transport interactions
struct ConsumeResult {
    bool consumed;
    AnnexConsumerRouter::SubjectCategory category;
    std::string diagnostics;
};

class AnnexConsumer {
public:
    // Core inbound consumption interface. 
    // Takes a raw subject string and the previously deserialized projection struct.
    // Does NOT return or interact with OmegaEventEnvelope.
    static ConsumeResult consume_projection(
        std::string_view subject, 
        const AnnexTransportProjection& payload) noexcept 
    {
        // 1. Identify Subject Routing Profile
        auto category = AnnexConsumerRouter::identify_category(subject);
        if (category == AnnexConsumerRouter::SubjectCategory::UNKNOWN) {
            return {false, category, "Subject routing category unknown: " + std::string(subject)};
        }

        // 2. Validate Canonical Fields and Version Guardrails
        auto validation = AnnexConsumerValidator::validate_payload(payload);
        if (!validation.is_valid) {
            return {false, category, "Validation Failed: " + validation.rejection_reason};
        }

        // 3. Optional Diagnostics: Expose numeric limitations transparently for downstream readers
        AnnexConsumerValidator::print_schema_warnings(validation);

        // 4. Enforce Cross-Subject Safety Guardrails
        // (A lifecycle consumer should never accidentally ingest a malformed event because of wide wildcard routing)
        if (category == AnnexConsumerRouter::SubjectCategory::INVALID && 
            payload.parse_status != vocab::ParseStatus::Error) {
             return {false, category, "Invalid taxonomy violation: valid parse status over invalid subject"};
        }

        if (category == AnnexConsumerRouter::SubjectCategory::LIFECYCLE && 
            payload.parse_status == vocab::ParseStatus::Error) {
             return {false, category, "Invalid taxonomy violation: parse error traversing lifecycle stream"};
        }

        if (category == AnnexConsumerRouter::SubjectCategory::CORRECTIONS &&
            !AnnexConsumerRouter::is_correction_event(payload.correction_type)) {
             return {false, category, "Invalid taxonomy violation: non-correction traversing correction stream"};
        }

        // Consumption successfully bound and validated. Real implementation passes to handlers.
        return {true, category, "Consumed successfully"};
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
