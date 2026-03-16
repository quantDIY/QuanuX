#pragma once

#include "quanux/omega/adapters/nasdaq/nasdaq_structs.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace nasdaq {

class NasdaqAdapter {
public:
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "NASDAQ_ITCH_OUCH_MOCK",
            .schema_compliance = {
                .version_string = "v1.0.0",
                .compatibility_note = "Provisional precision mapping active for shares and price.",
                .holds_deprecations = true
            },
            .time_proofs = {
                .proves_source_native = true,
                .proves_venue_gateway = true,
                .proves_receive_nic = false
            },
            .linkage_proofs = {
                .proves_busts = true,
                .proves_price_corrections = true,
                .proves_quantity_corrections = true,
                .maintains_deterministic_replay_state = true
            }
        };
    }

    // A broad semantic mapping function. Evaluates the ingress mock and normalizes it.
    static bool parse_ingress_message(
        const uint8_t* buffer, 
        size_t length, 
        core::OmegaEventEnvelope& out_envelope) noexcept 
    {
        if (length < sizeof(NasdaqIngressMock)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Missing Identity (Semantic Failure coverage)
            return false;
        }

        const auto* msg = reinterpret_cast<const NasdaqIngressMock*>(buffer);

        // 1. Identity Translation
        if (msg->order_reference_number == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Missing required venue identity
            return false;
        }
        
        out_envelope.identity.event_id = ids::EventId(msg->order_reference_number);
        
        // Emulate zero-copy boundaries over the fixed arrays
        size_t sym_len = 0;
        while (sym_len < 8 && msg->stock_symbol[sym_len] != ' ' && msg->stock_symbol[sym_len] != '\0') sym_len++;
        if (sym_len > 0) {
            // Unsafe string_view for real life unless backed, but sufficient for the map boundary mock
            out_envelope.identity.instrument_id = std::string_view(msg->stock_symbol, sym_len);
        }

        // 2. Semantics and Lifecycle State Translation
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success; // Assume success, downgrade on error

        switch (msg->message_type) {
            case 'A': // Add Order
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull; // Assuming it represents an active resting state here for mock purposes
                out_envelope.semantics.normalized_state = vocab::NormalizedState::New;
                break;
            case 'E': // Execute
            case 'C': // Execute with Price
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Filled;
                break;
            case 'X': // Cancel
                out_envelope.semantics.event_type = vocab::EventType::ExecutionFull;
                out_envelope.semantics.normalized_state = vocab::NormalizedState::Canceled;
                if (msg->cancel_reason == 'S') {
                    out_envelope.semantics.reason_code = vocab::NormalizedReasonCode::None;
                } else if (msg->cancel_reason == 'U') { // Supervisory
                    out_envelope.semantics.reason_code = vocab::NormalizedReasonCode::VenueTechnicalFault;
                }
                break;
            default:
                out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
                // Semantic Failure: Unknown state/message type
                return false; 
        }

        if (msg->side == 'B') out_envelope.semantics.side = vocab::OrderSide::Buy;
        else if (msg->side == 'S') out_envelope.semantics.side = vocab::OrderSide::Sell;

        // Numeric mappings - heavily provisional
        out_envelope.semantics.quantity = static_cast<double>(msg->shares);
        out_envelope.semantics.price = static_cast<double>(msg->price) / 10000.0; // 4-decimal implied

        // 3. Time Precedence
        if (msg->timestamp_nanos == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Malformed timestamp
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = msg->timestamp_nanos;

        // 4. Provenance & Survival (Strict Preservation)
        out_envelope.provenance.adapter_name = "NASDAQ_ITCH_OUCH_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";
        
        // Raw Ext Preservation: Push the pure message type into string-based survival
        static char type_str[2] = {0};
        type_str[0] = msg->message_type;
        out_envelope.extensions.venue_native_event_code = std::string_view(type_str, 1);

        // System MPID identity survival
        if (msg->mpid[0] != ' ' && msg->mpid[0] != '\0') {
            static char mpid_str[5] = {0};
            std::memcpy(mpid_str, msg->mpid, 4);
            static survival::TagValue mpid_tag{1000, std::string_view(mpid_str)};
            out_envelope.extensions.tags = std::span<const survival::TagValue>(&mpid_tag, 1);
        }

        return true;
    }
};

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
