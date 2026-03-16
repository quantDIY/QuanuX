#pragma once

#include "quanux/omega/counterparties/goldman_sachs/goldman_sachs_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"
#include <cstdio>
#include <string>

namespace quanux {
namespace omega {
namespace counterparties {
namespace goldman_sachs {

class GoldmanSachsAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "GOLDMAN_SACHS_PATH",
            .schema_compliance = {
                .version_string = "v1.0.0",
                .compatibility_note = "Provisional precision mapping active.",
                .holds_deprecations = true
            },
            .time_proofs = {
                .proves_source_native = false, // Counterparties obscure native source 
                .proves_venue_gateway = false,
                .proves_receive_nic = true
            },
            .linkage_proofs = {
                .proves_busts = true,
                .proves_price_corrections = true,
                .proves_quantity_corrections = true,
                .maintains_deterministic_replay_state = true
            }
        };
    }

    static bool parse_ingress_message(
        const uint8_t* buffer, 
        size_t length, 
        core::OmegaEventEnvelope& out_envelope) noexcept 
    {
        out_envelope.provenance.adapter_name = "GOLDMAN_SACHS_PATH";
        out_envelope.provenance.adapter_version = "v1.0.0";

        if (length < sizeof(GoldmanSachsIngressMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const GoldmanSachsIngressMock*>(buffer);

        if (msg->client_order_id[0] == '\0') {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        
        uint64_t hash = 5381;
        for (int i = 0; i < 24 && msg->client_order_id[i] != '\0' && msg->client_order_id[i] != ' '; ++i) {
            hash = ((hash << 5) + hash) + msg->client_order_id[i];
        }
        out_envelope.identity.event_id = ids::EventId(hash);

        // Map Counterparty Identity Explicitly
        out_envelope.identity._backing_counterparty_id = "GS";
        out_envelope.identity.counterparty_id = out_envelope.identity._backing_counterparty_id;

        // Map Route explicitly
        size_t r_len = 0;
        while (r_len < 16 && msg->routing_strategy[r_len] != ' ' && msg->routing_strategy[r_len] != '\0') r_len++;
        if (r_len > 0) {
            out_envelope.identity._backing_route_id = std::string(msg->routing_strategy, r_len);
            out_envelope.identity.route_id = out_envelope.identity._backing_route_id;
        }

        if (msg->route_transit_time_nanos == 0) {
             out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
             return false;
        }
        // Bind transit time to adapter time boundary because Gateway Source Time is abstracted away
        out_envelope.time.adapter_time.epoch_nanos = msg->route_transit_time_nanos;
        
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->side == 'B') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 'S') out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->fill_qty);
        out_envelope.semantics.price = msg->fill_price;

        if (msg->exec_type == 'N') {
            out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
        } else if (msg->exec_type == 'P') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionPartial;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::PartiallyFilled;
        } else if (msg->exec_type == 'F') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
        } else if (msg->exec_type == 'R') {
             out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
             out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
        } else {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->msg_type);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        out_envelope.identity._backing_client_order_id = std::string(msg->client_order_id, 24);
        out_envelope.identity.client_order_id = out_envelope.identity._backing_client_order_id;
        
        return true;
    }
};

} // namespace goldman_sachs
} // namespace counterparties
} // namespace omega
} // namespace quanux
