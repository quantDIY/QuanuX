#pragma once

#include <string>
#include <string_view>
#include "quanux/omega/omega_core/omega_event_linkage.hpp"

namespace quanux {
namespace omega {
namespace integration {

// Centralized routing taxonomy to prevent subject string manipulation bleed.
class AnnexConsumerRouter {
public:
    static constexpr std::string_view PREFIX_LIFECYCLE = "omega.events.lifecycle.v1.";
    static constexpr std::string_view PREFIX_CORRECTIONS = "omega.events.corrections.v1.";
    static constexpr std::string_view PREFIX_INVALID = "omega.events.invalid.v1.";
    static constexpr std::string_view PREFIX_CAPABILITY = "omega.events.capability.v1.";
    static constexpr std::string_view ROOT_SCHEMA = "omega.events.schema.v1"; // No adapter needed for generic schema annoucements

    // Given a route subject, determines its stream category. Consumer handlers map streams securely.
    enum class SubjectCategory {
        UNKNOWN,
        LIFECYCLE,
        CORRECTIONS,
        INVALID,
        CAPABILITY,
        SCHEMA
    };

    static SubjectCategory identify_category(std::string_view subject) noexcept {
        if (subject.starts_with(PREFIX_LIFECYCLE)) return SubjectCategory::LIFECYCLE;
        if (subject.starts_with(PREFIX_CORRECTIONS)) return SubjectCategory::CORRECTIONS;
        if (subject.starts_with(PREFIX_INVALID)) return SubjectCategory::INVALID;
        if (subject.starts_with(PREFIX_CAPABILITY)) return SubjectCategory::CAPABILITY;
        if (subject == ROOT_SCHEMA) return SubjectCategory::SCHEMA;
        return SubjectCategory::UNKNOWN;
    }

    // Determine if an event qualifies as a "correction" versus a standard lifecycle progression.
    static bool is_correction_event(core::CorrectionType type) noexcept {
        switch (type) {
            case core::CorrectionType::Bust:
            case core::CorrectionType::PriceCorrection:
            case core::CorrectionType::SizeCorrection:
            case core::CorrectionType::AccountCorrection:
            case core::CorrectionType::FullSupersession:
                return true;
            case core::CorrectionType::None:
            default:
                return false;
        }
    }

    // Builder methods used by publisher and validated against locally
    static std::string build_lifecycle_subject(std::string_view adapter_name) {
        return std::string(PREFIX_LIFECYCLE) + std::string(adapter_name);
    }
    
    static std::string build_correction_subject(std::string_view adapter_name) {
        return std::string(PREFIX_CORRECTIONS) + std::string(adapter_name);
    }

    static std::string build_invalid_subject(std::string_view adapter_name) {
        return std::string(PREFIX_INVALID) + std::string(adapter_name);
    }
    
    static std::string build_capability_subject(std::string_view adapter_name) {
        return std::string(PREFIX_CAPABILITY) + std::string(adapter_name);
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
