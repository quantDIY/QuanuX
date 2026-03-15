#pragma once

#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/evidence/raw_payload_hash.hpp"
#include "quanux/omega/survival/tag_value.hpp"
#include <cstring>

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

// A mock struct mimicking CME MDP3 SBE Execution Report layout for the bridge
#pragma pack(push, 1)
struct CmeExecutionReportSbe {
    uint64_t sequence_number;
    uint64_t order_id;
    uint64_t exec_id;
    char cl_ord_id[20];
    uint64_t transact_time;
    uint8_t template_id;
    uint8_t ord_status;  // 0=New, 1=PartiallyFilled, 2=Filled, 4=Canceled, 8=Rejected
    uint16_t unmapped_native_code; // SBE field that has no Omega translation
};
#pragma pack(pop)

class CmeAdapter {
public:
    // Zero-copy ingestion bridge. Parses a raw byte buffer and fills an OmegaEventEnvelope.
    static bool parse_execution_report(
        const uint8_t* raw_buffer, 
        std::size_t buffer_len, 
        core::OmegaEventEnvelope& out_envelope,
        survival::TagValue& out_survival_tag) noexcept 
    {
        if (buffer_len < sizeof(CmeExecutionReportSbe)) {
            return false;
        }

        const auto* msg = reinterpret_cast<const CmeExecutionReportSbe*>(raw_buffer);

        // Map Raw Evidence & Hash
        out_envelope.raw_evidence.data = raw_buffer;
        out_envelope.raw_evidence.size = buffer_len;
        
        // Compute payload hash inline
        out_envelope.payload_hash.hash_value = evidence::compute_fnv1a(raw_buffer, buffer_len);

        // Map Identities
        out_envelope.identity.event_id = ids::EventId(msg->sequence_number);
        
        // Using string_view for zero-copy string references
        out_envelope.identity.client_order_id = std::string_view(
            msg->cl_ord_id, 
            strnlen(msg->cl_ord_id, sizeof(msg->cl_ord_id))
        );

        // Map Time
        out_envelope.time.source_time.epoch_nanos = msg->transact_time;
        out_envelope.time.source_time.precedence = time::TimestampPrecedence::SourceNative;

        // Map Event & State
        switch (msg->ord_status) {
            case 0:
                out_envelope.event_type = vocab::EventType::OrderAcknowledged;
                out_envelope.normalized_state = vocab::NormalizedState::New;
                break;
            case 1:
                out_envelope.event_type = vocab::EventType::ExecutionPartial;
                out_envelope.normalized_state = vocab::NormalizedState::PartiallyFilled;
                break;
            case 2:
                out_envelope.event_type = vocab::EventType::ExecutionFull;
                out_envelope.normalized_state = vocab::NormalizedState::Filled;
                break;
            case 4:
                out_envelope.event_type = vocab::EventType::CancelReplaced; // Simplified
                out_envelope.normalized_state = vocab::NormalizedState::Canceled;
                break;
            case 8:
                out_envelope.event_type = vocab::EventType::OrderRejected;
                out_envelope.normalized_state = vocab::NormalizedState::Rejected;
                break;
            default:
                out_envelope.event_type = vocab::EventType::Unknown;
                out_envelope.normalized_state = vocab::NormalizedState::Unknown;
                break;
        }

        // Map Survival Tag (Proof of non-destructive normalisation)
        // Store the raw unmapped CME-specific native code using the TagValue structure
        if (msg->unmapped_native_code != 0) {
            out_survival_tag.tag_id = 9999; // Arbitrary custom Tag ID for demo
            
            // Reinterpret the memory address of the struct's specific field as the string_view
            // This preserves the exact bytes without copying them into a string.
            out_survival_tag.value = std::string_view(
                reinterpret_cast<const char*>(&msg->unmapped_native_code), 
                sizeof(msg->unmapped_native_code)
            );
        }

        return true;
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
