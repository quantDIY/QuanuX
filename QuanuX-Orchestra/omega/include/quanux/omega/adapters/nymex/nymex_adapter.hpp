#pragma once

#include "quanux/omega/adapters/nymex/nymex_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace nymex {

class NymexAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "NYMEX_MOCK",
            .schema_compliance = {
                .version_string = "v1.0.0",
                .compatibility_note = "NYMEX uses native double pricing. Still deprecation bounded by types::Price.",
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
        out_envelope.provenance.adapter_name = "NYMEX_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";

        if (length < sizeof(NymexExecutionMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const NymexExecutionMock*>(buffer);

        if (msg->trade_id[0] == '\0') {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        
        // Fast hash function for string IDs to enforce event boundaries
        uint64_t hash = 5381;
        for (int i = 0; i < 16 && msg->trade_id[i] != '\0'; ++i) {
            hash = ((hash << 5) + hash) + msg->trade_id[i]; 
        }
        out_envelope.identity.event_id = ids::EventId(hash);

        size_t sym_len = 0;
        while (sym_len < 6 && msg->contract_symbol[sym_len] != ' ' && msg->contract_symbol[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            out_envelope.identity._backing_instrument_id = std::string(msg->contract_symbol, sym_len);
            out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;
        }

        if (msg->transact_time_nanos == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->transact_time_nanos;

        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;

        if (msg->buy_sell == 'B') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->buy_sell == 'S') out_envelope.semantics.side = vocab::OrderSide::Sell;

        out_envelope.semantics.quantity = static_cast<double>(msg->quantity);
        out_envelope.semantics.price = msg->execution_price;

        switch (msg->execution_status) {
            case 0:
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case 1:
                out_envelope.semantics.event_type = vocab::EventType::CancelReplaced;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Canceled;
                break;
            case 2:
                out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
                break;
            default:
                out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
                return false;
        }

        char type_str[10];
        snprintf(type_str, sizeof(type_str), "%u", msg->transaction_type);
        out_envelope.extensions._backing_native_event_code = std::string(type_str);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        if (msg->execution_status == 2) {
             char err_str[10];
             snprintf(err_str, sizeof(err_str), "%u", msg->reject_code);
             out_envelope.extensions._backing_native_reason_code = std::string(err_str);
             out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
        }

        // Add raw string identifier mapping
        out_envelope.identity._backing_client_order_id = std::string(msg->trade_id, 16);
        out_envelope.identity.venue_order_id = out_envelope.identity._backing_client_order_id;

        return true;
    }
};

} // namespace nymex
} // namespace adapters
} // namespace omega
} // namespace quanux
