#pragma once

#include "quanux/omega/omega_vocab/event_type.hpp"
#include "quanux/omega/omega_vocab/normalized_state.hpp"
#include "quanux/omega/omega_vocab/normalized_reason_code.hpp"
#include "quanux/omega/omega_vocab/order_side.hpp"
#include "quanux/omega/omega_vocab/order_type.hpp"
#include "quanux/omega/omega_vocab/time_in_force.hpp"
#include "quanux/omega/omega_types/omega_numeric.hpp"

namespace quanux {
namespace omega {
namespace core {

struct OmegaEventSemantics {
    vocab::EventType event_type{vocab::EventType::Unknown};
    vocab::NormalizedState normalized_state{vocab::NormalizedState::Unknown};
    vocab::NormalizedReasonCode reason_code{vocab::NormalizedReasonCode::None};
    
    vocab::OrderSide side{vocab::OrderSide::Unknown};
    vocab::OrderType order_type{vocab::OrderType::Unknown};
    vocab::TimeInForce time_in_force{vocab::TimeInForce::Unknown};

    // Utilizing provisional type aliases to avoid entrenching naked double semantics.
    types::Quantity quantity{0.0};
    types::Quantity cum_qty{0.0};
    types::Quantity leaves_qty{0.0};
    types::Price price{0.0};
    types::Price last_px{0.0};
    types::Quantity last_qty{0.0};
};

} // namespace core
} // namespace omega
} // namespace quanux
