#pragma once

#include "quanux/omega/counterparties/jpmorgan/jpmorgan_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"
#include <cstdio>
#include <string>

namespace quanux {
namespace omega {
namespace counterparties {
namespace jpmorgan {

class JPMorganAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "JPMORGAN_PATH",
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
        out_envelope.provenance.adapter_name = "JPMORGAN_PATH";
        out_envelope.provenance.adapter_version = "v1.0.0";

        if (length < sizeof(JPMorganIngressMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const JPMorganIngressMock*>(buffer);

        if (msg->jpm_order_id[0] == '\0') {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        
        uint64_t hash = 5381;
        for (int i = 0; i < 32 && msg->jpm_order_id[i] != '\0' && msg->jpm_order_id[i] != ' '; ++i) {
            hash = ((hash << 5) + hash) + msg->jpm_order_id[i];
        }
        out_envelope.identity.event_id = ids::EventId(hash);

        // Explicit Counterparty Identity
        out_envelope.identity._backing_counterparty_id = "JPM";
        out_envelope.identity.counterparty_id = out_envelope.identity._backing_counterparty_id;

        out_envelope.identity._backing_client_order_id = std::string(msg->jpm_order_id, 32);
        out_envelope.identity.client_order_id = out_envelope.identity._backing_client_order_id;
        out_envelope.identity.venue_order_id = out_envelope.identity._backing_client_order_id;

        // Extract Explicit Venue independently of Counterparty Identity
        size_t v_len = 0;
        while (v_len < 8 && msg->execution_venue[v_len] != ' ' && msg->execution_venue[v_len] != '\0') v_len++;
        if (v_len > 0) {
            std::string ven_str(msg->execution_venue, v_len);
            out_envelope.extensions._backing_opaque_extension_blob = ven_str;
            out_envelope.provenance.source_protocol = out_envelope.extensions._backing_opaque_extension_blob;
        }

        if (msg->router_timestamp_nanos == 0) {
             out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
             return false;
        }
        out_envelope.time.adapter_time.epoch_nanos = msg->router_timestamp_nanos;
        
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->side == 'B') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 'S') out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->execution_quantity);
        out_envelope.semantics.price = msg->execution_price;

        if (msg->order_status == 'N') {
            out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
        } else if (msg->order_status == 'P') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionPartial;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::PartiallyFilled;
        } else if (msg->order_status == 'F') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
        } else if (msg->order_status == 'R') {
             out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
             out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
             
             size_t rlen = 0;
             while (rlen < 32 && msg->reject_reason_string[rlen] != '\0') rlen++;
             if (rlen > 0) {
                 out_envelope.extensions._backing_native_reason_code = std::string(msg->reject_reason_string, rlen);
                 out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
             }
        } else {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->payload_code);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        return true;
    }
};

} // namespace jpmorgan
} // namespace counterparties
} // namespace omega
} // namespace quanux
