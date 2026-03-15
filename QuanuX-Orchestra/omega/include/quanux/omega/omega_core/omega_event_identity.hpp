#pragma once

#include "quanux/omega/omega_ids/event_id.hpp"
#include <string_view>

namespace quanux {
namespace omega {
namespace core {

struct OmegaEventIdentity {
    ids::EventId event_id;
    // IDs must point to canonical representation of strings for zero-copy downstream.
    // The lifetimes of the underlying buffers must outlive this view.
    std::string_view venue_order_id;
    std::string_view client_order_id;
    std::string_view execution_id;
    
    // Core business boundaries
    std::string_view account_id;
    std::string_view strategy_id;
    std::string_view instrument_id;
    std::string_view firm_order_id;

    [[nodiscard]] constexpr bool has_event_id() const noexcept {
        return event_id.is_valid();
    }
};

} // namespace core
} // namespace omega
} // namespace quanux
