#pragma once

#include "quanux/omega/omega_core/omega_event_identity.hpp"
#include "quanux/omega/omega_core/omega_event_time.hpp"
#include "quanux/omega/omega_core/omega_event_linkage.hpp"
#include "quanux/omega/omega_core/omega_event_semantics.hpp"
#include "quanux/omega/omega_core/omega_event_provenance.hpp"
#include "quanux/omega/omega_core/omega_event_extensions.hpp"

namespace quanux {
namespace omega {
namespace core {

// The canonical normalized event contract
class OmegaEventEnvelope {
public:
    OmegaEventEnvelope() = default;

    // Identities & Addressing
    OmegaEventIdentity identity;
    
    // Time & Lineage
    OmegaEventTime time;
    OmegaEventLinkage linkage;

    // First-Class Semantic Boundaries
    OmegaEventSemantics semantics;
    OmegaEventProvenance provenance;
    OmegaEventExtensions extensions;
};

} // namespace core
} // namespace omega
} // namespace quanux
