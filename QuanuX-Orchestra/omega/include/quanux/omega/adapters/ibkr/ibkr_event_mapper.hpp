#pragma once

#include <string_view>
#include "quanux/omega/omega_core/omega_event_semantics.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrEventMapper {
public:
    static void map_status(std::string_view ibkr_status, core::OmegaEventSemantics& semantics) noexcept {
        if (ibkr_status == "Submitted" || ibkr_status == "PreSubmitted") {
            semantics.event_type = vocab::EventType::OrderAcknowledged;
            semantics.normalized_state = vocab::NormalizedState::New;
        } else if (ibkr_status == "Filled") {
            semantics.event_type = vocab::EventType::ExecutionFull;
            semantics.normalized_state = vocab::NormalizedState::Filled;
        } else if (ibkr_status == "Cancelled") {
            semantics.event_type = vocab::EventType::CancelReplaced; // Simplified hook mimicking CME
            semantics.normalized_state = vocab::NormalizedState::Canceled;
        } else if (ibkr_status == "Inactive") {
            semantics.event_type = vocab::EventType::OrderRejected;
            semantics.normalized_state = vocab::NormalizedState::Rejected;
        } else {
            // Partial filled is typically notified natively via Execution logic 
            // without a dedicated "PartiallyFilled" string, but handled heuristically.
            // For now, assume unmapped string defaults to untranslated unknown.
            semantics.event_type = vocab::EventType::Unknown;
            semantics.normalized_state = vocab::NormalizedState::Unknown;
        }
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
