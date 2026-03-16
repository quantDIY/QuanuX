#pragma once

#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/evidence/raw_payload_hash.hpp"
#include "quanux/omega/survival/tag_value.hpp"
#include <cstring>

#include "quanux/omega/adapters/cme/cme_event_mapper.hpp"
#include "quanux/omega/adapters/cme/cme_reason_mapper.hpp"
#include "quanux/omega/adapters/cme/cme_field_mapper.hpp"
#include "quanux/omega/adapters/cme/cme_timestamp_mapper.hpp"
#include "quanux/omega/adapters/cme/cme_correction_mapper.hpp"
#include "quanux/omega/adapters/cme/cme_survival_mapper.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

// WARNING: NUMERIC LIMITATIONS
// This adapter utilizes quanux::omega::types::Price and Quantity aliases 
// for extracting numeric values. These are currently provisional 'double' 
// types and explicitly DO NOT provide accounting-grade or reconciliation-grade 
// precision guarantees.
// Downstream consumers must not canonize these abstractions as the final model.

// A mock struct mimicking CME MDP3 SBE Execution Report layout for the bridge
#pragma pack(push, 1)
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"
#pragma pack(pop)

class CmeAdapter {
public:
    // Expose the formal capability proof for this adapter
    static capability::SourceCapabilityProfile get_capability_profile() noexcept {
        return capability::SourceCapabilityProfile{
            .adapter_name = "CME_iLink3",
            .schema_compliance = {"v1.0.0", "Provisional precision mapping active.", false},
            .time_proofs = {true, false, false}, // Proves source time only typically
            .linkage_proofs = {true, true, false, true} // Proves busts, price corrects, and deterministic replay
        };
    }

    // Zero-copy ingestion bridge. Parses a raw byte buffer and fills an OmegaEventEnvelope.
    static bool parse_execution_report(
        const uint8_t* raw_buffer, 
        std::size_t buffer_len, 
        core::OmegaEventEnvelope& out_envelope,
        survival::TagValue& out_survival_tag) noexcept 
    {
        if (buffer_len < sizeof(CmeExecutionReportSbe)) {
            out_envelope.provenance.parse_status = vocab::ParseStatus::Error;
            out_envelope.provenance.adapter_name = "CME_iLink3";
            return false;
        }

        const auto* msg = reinterpret_cast<const CmeExecutionReportSbe*>(raw_buffer);

        // 1. Map Provenance & Lineage Identity
        out_envelope.provenance.adapter_name = "CME_iLink3";
        out_envelope.provenance.adapter_version = "v2.1";
        out_envelope.provenance.parse_status = vocab::ParseStatus::Success;
        out_envelope.provenance.raw_evidence.data = raw_buffer;
        out_envelope.provenance.raw_evidence.size = buffer_len;
        out_envelope.provenance.payload_hash.hash_value = evidence::compute_fnv1a(raw_buffer, buffer_len);

        // 2. Map Sub-components using discrete, testable mappers
        CmeFieldMapper::map_fields(msg, out_envelope.semantics, out_envelope.identity);
        CmeEventMapper::map_status(msg->ord_status, out_envelope.semantics);
        CmeReasonMapper::map_reason(msg->ord_status, msg->md_error_code, out_envelope.semantics);
        CmeTimestampMapper::map_time(msg, out_envelope.time);
        CmeCorrectionMapper::map_corrections(msg, out_envelope.linkage);
        CmeSurvivalMapper::map_survival_fields(msg, out_envelope.extensions, out_survival_tag);

        return true;
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
