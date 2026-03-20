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
                .compatibility_note = "Provisional precision mapping active.",
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

        // ITCH 5.0 Endianness Conversions (Big-Endian to Local Execution Little-Endian)
        uint64_t timestamp = __builtin_bswap64(msg->timestamp_nanos);
        uint64_t order_ref = __builtin_bswap64(msg->order_reference_number);
        uint16_t locate = __builtin_bswap16(msg->stock_locate);
        uint32_t net_shares = __builtin_bswap32(msg->shares);
        uint32_t net_price = __builtin_bswap32(msg->price);

        out_envelope.identity._backing_venue_id = "NASDAQ";
        out_envelope.identity.venue_id = out_envelope.identity._backing_venue_id;

        // Semantic Failure: Missing required venue identity reference
        if (order_ref == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            return false;
        }
        out_envelope.identity.event_id = ids::EventId(order_ref);
        
        // StockLocate Directory Synchronization Coverage (Pre-Market Mapping Simulation)
        if (locate == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Dropped undefined mapped StockLocate identifier
            return false;
        }
        
        // Native ITCH mapping resolves String Identifiers explicitly through the integer map (simulated inline)
        out_envelope.identity._backing_instrument_id = "ITCH_LOCATE_" + std::to_string(locate);
        out_envelope.identity.instrument_id = out_envelope.identity._backing_instrument_id;

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

        // Numeric mappings implementing explicit conversions
        out_envelope.semantics.quantity = static_cast<double>(net_shares);
        out_envelope.semantics.price = static_cast<double>(net_price) / 10000.0; // 4-decimal implied

        // 3. Time Precedence
        if (timestamp == 0) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            // Semantic Failure: Malformed timestamp
            return false;
        }
        out_envelope.time.source_time.epoch_nanos = timestamp;

        // 4. Provenance & Survival (Strict Preservation)
        out_envelope.provenance.adapter_name = "NASDAQ_ITCH_OUCH_MOCK";
        out_envelope.provenance.adapter_version = "v1.0.0";
        
        // Raw Ext Preservation: Push the pure message type into string-based survival
        out_envelope.extensions._backing_native_event_code = std::string(1, msg->message_type);
        out_envelope.extensions.venue_native_event_code = out_envelope.extensions._backing_native_event_code;

        // System MPID identity survival
        if (msg->mpid[0] != ' ' && msg->mpid[0] != '\0') {
            out_envelope.extensions.emplace_inline_tag(survival::TagValue{1000, std::string_view(msg->mpid, 4)});
        }

        return true;
    }
};

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
