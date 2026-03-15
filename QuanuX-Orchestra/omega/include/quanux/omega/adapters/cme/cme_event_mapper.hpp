#pragma once

#include "quanux/omega/omega_core/omega_event_semantics.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeEventMapper {
public:
    static void map_status(uint8_t cme_ord_status, core::OmegaEventSemantics& semantics) noexcept {
        switch (cme_ord_status) {
            case 0:
                semantics.event_type = vocab::EventType::OrderAcknowledged;
                semantics.normalized_state = vocab::NormalizedState::New;
                break;
            case 1:
                semantics.event_type = vocab::EventType::ExecutionPartial;
                semantics.normalized_state = vocab::NormalizedState::PartiallyFilled;
                break;
            case 2:
                semantics.event_type = vocab::EventType::ExecutionFull;
                semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case 4:
                semantics.event_type = vocab::EventType::CancelReplaced; // Simplified hook
                semantics.normalized_state = vocab::NormalizedState::Canceled;
                break;
            case 8:
                semantics.event_type = vocab::EventType::OrderRejected;
                semantics.normalized_state = vocab::NormalizedState::Rejected;
                break;
            default:
                semantics.event_type = vocab::EventType::Unknown;
                semantics.normalized_state = vocab::NormalizedState::Unknown;
                break;
        }
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
