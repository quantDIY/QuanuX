#pragma once

#include "quanux/omega/adapters/cbot/cbot_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cbot {

class CbotAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "CBOT_MOCK",
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
        out_envelope.identity._backing_venue_id = "CBOT";
        out_envelope.identity.venue_id = out_envelope.identity._backing_venue_id;

        // Map Sidend identity regardless of parse outcome so dead letter maps properly
        out_envelope.provenance.adapter_name = "CBOT_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";

        if (length < sizeof(CbotExecutionMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const CbotExecutionMock*>(buffer);

        // Identity Validation
        if (msg->cbot_order_id == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.identity.event_id = ids::EventId(msg->cbot_order_id);

        size_t sym_len = 0;
        while (sym_len < 4 && msg->product_group[sym_len] != ' ' && msg->product_group[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            out_envelope.identity._backing_instrument_id = std::string(msg->product_group, sym_len);
            out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;
        }

        // Time mapping
        if (msg->transact_time_nanos == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->transact_time_nanos;

        // Semantics Mapping
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->side == 1) out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 2) out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->fill_qty);
        out_envelope.semantics.price = static_cast<double>(msg->fill_price) / 100000000.0; // 8-decimal scaling

        switch (msg->status_code) {
            case '0':
                out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
                break;
            case '4':
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case '8':
                out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
                break;
            default:
                out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
                return false;
        }

        // Provenance mappings adhering to Thread-Safe Lifetime rules (Phase 8)
        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->msg_type);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        if (msg->status_code == '8') {
             char err_str[10];
             snprintf(err_str, sizeof(err_str), "%u", msg->error_code);
             out_envelope.extensions._backing_native_reason_code = std::string(err_str);
             out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
        }

        return true;
    }
};

} // namespace cbot
} // namespace adapters
} // namespace omega
} // namespace quanux
