#pragma once

#include <string_view>
#include <cstring>
#include "quanux/omega/omega_core/omega_event_semantics.hpp"
#include "quanux/omega/omega_core/omega_event_identity.hpp"
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeFieldMapper {
public:
    static void map_fields(const CmeExecutionReportSbe* msg, core::OmegaEventSemantics& semantics, core::OmegaEventIdentity& identity) noexcept {
        // Map Identities
        identity.event_id = ids::EventId(msg->sequence_number);
        
        // Zero-copy string references
        identity.client_order_id = std::string_view(msg->cl_ord_id, strnlen(msg->cl_ord_id, sizeof(msg->cl_ord_id)));
        
        identity._backing_venue_id = "CME";
        identity.venue_id = identity._backing_venue_id;
        
        // Mapped directly from the placeholder struct to semantics
        // Assumes later SBE structures will have pricing and quantity fields
        semantics.quantity = msg->order_qty_placeholder; 
        semantics.price = msg->price_placeholder;
        
        // Map Side (mocked for now, assuming 1=Buy, 2=Sell in CME SBE)
        if (msg->side == 1) {
            semantics.side = vocab::OrderSide::Buy;
        } else if (msg->side == 2) {
            semantics.side = vocab::OrderSide::Sell;
        } else {
            semantics.side = vocab::OrderSide::Unknown;
        }
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
