#pragma once

#include "quanux/omega/omega_vocab/event_type.hpp"
#include "quanux/omega/omega_vocab/normalized_state.hpp"
#include "quanux/omega/omega_vocab/normalized_reason_code.hpp"
#include "quanux/omega/omega_vocab/order_side.hpp"
#include "quanux/omega/omega_vocab/order_type.hpp"
#include "quanux/omega/omega_vocab/time_in_force.hpp"

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

    // TODO: Replace 'double' with an appropriate precise numeric type 
    // (e.g., fixed-point / decimal-oriented type) to avoid floating-point 
    // ambiguity in post-trade accounting and reconstruction contexts.
    // Double is temporarily used strictly as a placeholder.
    double quantity{0.0};
    double cum_qty{0.0};
    double leaves_qty{0.0};
    double price{0.0};
    double last_px{0.0};
    double last_qty{0.0};
};

} // namespace core
} // namespace omega
} // namespace quanux
