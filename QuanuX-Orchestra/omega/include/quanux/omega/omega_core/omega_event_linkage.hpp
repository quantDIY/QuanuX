#pragma once

#include "quanux/omega/omega_ids/event_id.hpp"
#include <cstdint>

namespace quanux {
namespace omega {
namespace core {

enum class CorrectionType : uint8_t {
    None = 0,
    Bust = 1,
    PriceCorrection = 2,
    SizeCorrection = 3,
    AccountCorrection = 4,
    FullSupersession = 5
};

struct OmegaEventLinkage {
    ids::EventId prior_event_id;      // ID of the event this logically follows in a chain
    ids::EventId supersedes_event_id; // ID of the event this directly replaces/busts/corrects
    CorrectionType correction_type{CorrectionType::None};

    [[nodiscard]] constexpr bool is_correction() const noexcept {
        return correction_type != CorrectionType::None;
    }

    [[nodiscard]] constexpr bool has_prior_event() const noexcept {
        return prior_event_id.is_valid();
    }
    
    [[nodiscard]] constexpr bool supersedes_prior() const noexcept {
        return supersedes_event_id.is_valid();
    }
};

} // namespace core
} // namespace omega
} // namespace quanux
