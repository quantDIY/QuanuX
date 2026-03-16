#pragma once

#include "quanux/omega/adapters/comex/comex_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace comex {

class ComexAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "COMEX_MOCK",
            .schema_compliance = {
                .version_string = "v1.0.0",
                .compatibility_note = "Provisional precision mapping active.",
                .holds_deprecations = true
            },
            .time_proofs = {
                .proves_source_native = true,
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
        // Bind identity regardless of outcome for Dead-Letter routing safety
        out_envelope.provenance.adapter_name = "COMEX_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";
        out_envelope.identity._backing_venue_id = "COMEX";
        out_envelope.identity.venue_id = out_envelope.identity._backing_venue_id;

        if (length < sizeof(ComexMatchReport)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const ComexMatchReport*>(buffer);

        if (msg->comex_order_id == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.identity.event_id = ids::EventId(msg->comex_order_id);

        size_t sym_len = 0;
        while (sym_len < 4 && msg->product_group[sym_len] != ' ' && msg->product_group[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            out_envelope.identity._backing_instrument_id = std::string(msg->product_group, sym_len);
            out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;
        }

        if (msg->transact_time_nanos == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->transact_time_nanos;

        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->side == 'B') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 'S') out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->qty);
        out_envelope.semantics.price = static_cast<double>(msg->price) / 100000000.0;

        switch (msg->order_state) {
            case 'N':
                out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
                break;
            case 'F':
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case 'R':
                out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
                break;
            default:
                out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
                return false;
        }

        // Thread-Safe Survival Fields
        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->msg_type);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        if (msg->order_state == 'R') {
             char err_str[10];
             snprintf(err_str, sizeof(err_str), "%u", msg->reject_reason);
             out_envelope.extensions._backing_native_reason_code = std::string(err_str);
             out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
        }

        return true;
    }
};

} // namespace comex
} // namespace adapters
} // namespace omega
} // namespace quanux
