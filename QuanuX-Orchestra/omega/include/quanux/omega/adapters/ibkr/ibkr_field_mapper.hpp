#pragma once

#include <string_view>
#include "quanux/omega/omega_core/omega_event_semantics.hpp"
#include "quanux/omega/omega_core/omega_event_identity.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_structs.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrFieldMapper {
public:
    static void map_fields(const IbkrExecutionReportMock* msg, core::OmegaEventSemantics& semantics, core::OmegaEventIdentity& identity) noexcept {
        // 1. Core Semantics
        // Using provisional proxies Price and Quantity
        semantics.quantity = static_cast<types::Quantity>(msg->shares);
        semantics.price = static_cast<types::Price>(msg->price);
        semantics.cum_qty = static_cast<types::Quantity>(msg->cumQty);
        semantics.last_px = static_cast<types::Price>(msg->avgPrice);

        std::string_view _side(msg->side);
        if (_side == "BOT") {
            semantics.side = vocab::OrderSide::Buy;
        } else if (_side == "SLD") {
            semantics.side = vocab::OrderSide::Sell;
        } else {
            semantics.side = vocab::OrderSide::Unknown;
        }
        
        // 2. Identity Translation
        // We use reqId directly or combine it, assuming mock layout places it in orderId
        identity.event_id.value = static_cast<uint64_t>(msg->reqId);
        
        // Zero-copy string spanning
        identity.client_order_id = std::string_view(msg->execId);
        identity._backing_instrument_id = msg->exchange;
        identity.instrument_id = identity._backing_instrument_id;
        identity.account_id = std::string_view(msg->acctNumber);
        
        identity._backing_route_id = "IBKR";
        identity.route_id = identity._backing_route_id;
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
