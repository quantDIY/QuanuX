#pragma once

#include "quanux/omega/omega_core/omega_event_identity.hpp"
#include "quanux/omega/omega_core/omega_event_time.hpp"
#include "quanux/omega/omega_core/omega_event_linkage.hpp"
#include "quanux/omega/omega_vocab/event_type.hpp"
#include "quanux/omega/omega_vocab/normalized_state.hpp"
#include "quanux/omega/omega_vocab/normalized_reason_code.hpp"
#include "quanux/omega/evidence/raw_payload_ref.hpp"
#include "quanux/omega/evidence/raw_payload_hash.hpp"

namespace quanux {
namespace omega {
namespace core {

// The canonical normalized event contract
class OmegaEventEnvelope {
public:
    OmegaEventEnvelope() = default;

    // Identities & Addressing
    OmegaEventIdentity identity;
    
    // Time & Provenance
    OmegaEventTime time;
    evidence::RawPayloadRef raw_evidence;
    evidence::RawPayloadHash payload_hash;

    // Lineage & Corrections
    OmegaEventLinkage linkage;

    // Semantics
    vocab::EventType event_type{vocab::EventType::Unknown};
    vocab::NormalizedState normalized_state{vocab::NormalizedState::Unknown};
    vocab::NormalizedReasonCode reason_code{vocab::NormalizedReasonCode::None};
};

} // namespace core
} // namespace omega
} // namespace quanux
