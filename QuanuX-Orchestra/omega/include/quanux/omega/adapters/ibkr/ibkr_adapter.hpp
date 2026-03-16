#pragma once

#include <span>
#include <cstdint>
#include <string_view>

#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/evidence/raw_payload_hash.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_structs.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_event_mapper.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_reason_mapper.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_field_mapper.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_timestamp_mapper.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_correction_mapper.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_survival_mapper.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

// WARNING: NUMERIC LIMITATIONS
// This adapter utilizes quanux::omega::types::Price and Quantity aliases 
// for extracting numeric values. These are currently provisional 'double' 
// types and explicitly DO NOT provide accounting-grade or reconciliation-grade 
// precision guarantees.
// Downstream consumers must not canonize these abstractions as the final model.

class IbkrAdapter {
public:
    static std::string_view get_adapter_name() noexcept { return "IBKR_TWS_API"; }
    static std::string_view get_adapter_version() noexcept { return "v1.0"; }

    // Expose the formal capability proof for this adapter
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "IBKR_TWS_API",
            .schema_compliance = {"v1.0.0", "Provisional precision mapping active.", false},
            .time_proofs = {false, false, false}, // Text API often makes source times unpredictable vs gateway times
            .linkage_proofs = {true, true, false, false} // Cannot guarantee deterministic replay from disconnected gaps
        };
    }

    // Zero-copy ingestion bridge. Parses a mock IBKR struct and fills an OmegaEventEnvelope.
    static bool parse_execution_report(
        const uint8_t* payload, 
        size_t length, 
        core::OmegaEventEnvelope& envelope,
        survival::TagValue& shared_tag) noexcept 
    {
        if (length < sizeof(IbkrExecutionReportMock)) {
            envelope.provenance.parse_status = vocab::ParseStatus::Error;
            envelope.provenance.adapter_name = get_adapter_name();
            return false;
        }

        const IbkrExecutionReportMock* msg = reinterpret_cast<const IbkrExecutionReportMock*>(payload);

        // 1. Traceability & Provenance
        envelope.provenance.raw_evidence = evidence::RawPayloadRef{payload, length};
        envelope.provenance.payload_hash.hash_value = evidence::compute_fnv1a(payload, length);
        envelope.provenance.adapter_name = get_adapter_name();
        envelope.provenance.adapter_version = get_adapter_version();

        // 2. Semantics & Identity
        IbkrEventMapper::map_status(msg->status, envelope.semantics);
        IbkrReasonMapper::map_reason(msg->status, msg->whyHeld, msg->errorCode, envelope.semantics);
        
        IbkrFieldMapper::map_fields(msg, envelope.semantics, envelope.identity);
        IbkrTimestampMapper::map_time(msg, envelope.time);
        
        IbkrCorrectionMapper::map_linkage(msg, envelope.linkage);
        IbkrSurvivalMapper::map_extensions(msg, envelope.extensions, shared_tag);

        envelope.provenance.parse_status = vocab::ParseStatus::Success;

        return true;
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
