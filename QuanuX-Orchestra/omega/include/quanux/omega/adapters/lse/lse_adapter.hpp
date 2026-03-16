#pragma once

#include "quanux/omega/adapters/lse/lse_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace lse {

class LseAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "LSE_MOCK",
            .schema_compliance = {
                .version_string = "v1.0.0",
                .compatibility_note = "Provisional precision mapping active.",
                .holds_deprecations = true
            },
            .time_proofs = {
                .proves_source_native = true,
                .proves_venue_gateway = true,
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
        out_envelope.provenance.adapter_name = "LSE_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";
        out_envelope.identity._backing_venue_id = "LSE";
        out_envelope.identity.venue_id = out_envelope.identity._backing_venue_id;

        if (length < sizeof(LseExecutionMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const LseExecutionMock*>(buffer);

        if (msg->order_id[0] == '\0') {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        
        uint64_t hash = 5381;
        for (int i = 0; i < 20 && msg->order_id[i] != '\0' && msg->order_id[i] != ' '; ++i) {
            hash = ((hash << 5) + hash) + msg->order_id[i];
        }
        out_envelope.identity.event_id = ids::EventId(hash);

        size_t sym_len = 0;
        while (sym_len < 12 && msg->instrument_id[sym_len] != ' ' && msg->instrument_id[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            out_envelope.identity._backing_instrument_id = std::string(msg->instrument_id, sym_len);
            out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;
        }

        if (msg->transact_time_nanos == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->transact_time_nanos;

        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->side == '1') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == '2') out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->exec_qty);
        out_envelope.semantics.price = msg->exec_price;

        if (msg->exec_type[0] == '0') {
            out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
        } else if (msg->exec_type[0] == '1') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionPartial;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::PartiallyFilled;
        } else if (msg->exec_type[0] == '2') {
            out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
            out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
        } else if (msg->exec_type[0] == '8') {
             out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
             out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
        } else {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->message_type);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        if (msg->exec_type[0] == '8') {
            size_t rlen = 0;
            while (rlen < 4 && msg->reject_code[rlen] != ' ' && msg->reject_code[rlen] != '\0') rlen++;
            if (rlen > 0) {
                out_envelope.extensions._backing_native_reason_code = std::string(msg->reject_code, rlen);
                out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
            }
        }

        out_envelope.identity._backing_client_order_id = std::string(msg->order_id, 20);
        out_envelope.identity.venue_order_id = out_envelope.identity._backing_client_order_id;

        return true;
    }
};

} // namespace lse
} // namespace adapters
} // namespace omega
} // namespace quanux
