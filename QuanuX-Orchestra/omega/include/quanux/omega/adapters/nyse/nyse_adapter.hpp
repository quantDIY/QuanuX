#pragma once

#include "quanux/omega/adapters/nyse/nyse_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace nyse {

class NyseAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "NYSE_PILLAR_MOCK",
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
        if (length < sizeof(NysePillarMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }

        const auto* msg = reinterpret_cast<const NysePillarMock*>(buffer);

        out_envelope.identity._backing_venue_id = "NYSE";
        out_envelope.identity.venue_id = out_envelope.identity._backing_venue_id;
        
        // Identity Translation (Semantic checks first)
        if (msg->me_order_id == 0 && msg->client_order_id == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Impossible identity ambiguity
            return false;
        }

        out_envelope.identity.event_id = ids::EventId(msg->me_order_id != 0 ? msg->me_order_id : msg->client_order_id);
        
        size_t sym_len = 0;
        while (sym_len < 11 && msg->symbol[sym_len] != ' ' && msg->symbol[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            out_envelope.identity._backing_instrument_id = std::string(msg->symbol, sym_len);
            out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;
        }

        // 2. Lifecycle Mapping
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;
        
        if (msg->msg_type != 0x200 && msg->msg_type != 0x201 && msg->msg_type != 0x202 && msg->msg_type != 0x210) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Unknown structural ingress classification
            return false;
        }

        // Execution status normalization
        switch (msg->execution_status) {
            case '0': 
                out_envelope.semantics.event_type = vocab::EventType::OrderAcknowledged;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
                break;
            case '1': 
                out_envelope.semantics.event_type = vocab::EventType::ExecutionPartial;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::PartiallyFilled;
                break;
            case '2': 
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case '4': 
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Canceled;
                out_envelope.semantics.reason_code = vocab::NormalizedReasonCode::None; // Typical mapping assumption
                break;
            case '8': 
                out_envelope.semantics.event_type = vocab::EventType::OrderRejected;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Rejected;
                // Preserve the native reason code in extensions instead of hard-typing everything
                break;
            default:
                out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
                // Semantic Failure: Unmapped/Contradictory state enumeration
                return false;
        }

        if (msg->side == 1) out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 2 || msg->side == 3) out_envelope.semantics.side = vocab::OrderSide::Sell; // Coercing short sell to explicit intent

        // 3. Numeric Projections (Provisional displacement)
        out_envelope.semantics.quantity = static_cast<double>(msg->qty);
        out_envelope.semantics.price = static_cast<double>(msg->price) / 100000000.0; // 8-decimal native mapped to double

        // 4. Time
        if (msg->source_time_ns == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->source_time_ns;

        // 5. Provenance Bounds
        out_envelope.provenance.adapter_name = "NYSE_PILLAR_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";
        
        char msg_code[5] = {0};
        snprintf(msg_code, sizeof(msg_code), "%04X", msg->msg_type);
        out_envelope.extensions._backing_native_event_code = std::string(msg_code);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        // Keep raw error descriptions mapped for post-trade survival
        if (msg->execution_status == '8') {
             out_envelope.extensions._backing_native_reason_code = std::string(msg->reason_code, 4);
             out_envelope.extensions.venue_native_reason_code = out_envelope.extensions._backing_native_reason_code;
        }

        return true;
    }
};

} // namespace nyse
} // namespace adapters
} // namespace omega
} // namespace quanux
