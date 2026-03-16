#pragma once

#include <string>
#include <string_view>
#include <vector>
#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_capability/source_capability.hpp"
#include "quanux/omega/integration/annex_consumer_router.hpp"

namespace quanux {
namespace omega {
namespace integration {

// -----------------------------------------------------------------------------
// The Governed Transport Projection
// This struct explicitly disconnects the C++ zero-copy internal ring-buffer 
// structures from the outbound network serialization boundary.
// 
// Its shape strictly mirrors 'omega_schema.graphql'.
// -----------------------------------------------------------------------------
struct AnnexTransportProjection {
    // Exact schema version for payload-level contract identification
    std::string schema_version;

    // Identity
    std::string event_id;
    std::string client_order_id;
    std::string firm_order_id;
    std::string instrument_id;
    std::string account_id;
    std::string strategy_id;

    // Semantics
    vocab::EventType event_type;
    vocab::NormalizedState normalized_state;
    vocab::NormalizedReasonCode reason_code;
    vocab::OrderSide side;
    vocab::OrderType order_type;
    vocab::TimeInForce time_in_force;
    
    // Provisional Numeric Fields (Explicitly documented in Schema)
    types::Quantity quantity;
    types::Quantity cum_qty;
    types::Quantity leaves_qty;
    types::Price price;
    types::Price last_px;
    types::Quantity last_qty;

    // Time Precedence Nodes (converted to standard Unix Nanos inside the struct)
    uint64_t source_time_nanos;
    uint64_t venue_time_nanos;
    uint64_t receive_time_nanos;
    uint64_t adapter_time_nanos;
    uint64_t normalization_time_nanos;

    // Linkage
    core::CorrectionType correction_type;

    // Provenance
    std::string payload_hash; // Cryptographic Hash string format
    std::string adapter_name;
    std::string adapter_version;
    vocab::ParseStatus parse_status;

    // Survival Extensions
    std::string venue_native_event_code;
    std::string venue_native_reason_code;
    std::vector<std::pair<int, std::string>> survival_tags;
};

// Represents a routed payload ready for JetStream (Subject + Serialized Body)
struct AnnexPublishRoute {
    std::string subject;
    AnnexTransportProjection payload;
};

class AnnexPublisher {
public:
    // Routes an internal envelope into a network-ready projection
    static AnnexPublishRoute project_and_route(
        const core::OmegaEventEnvelope& env, 
        const capability::SourceCapabilityProfile& profile) noexcept 
    {
        AnnexPublishRoute route;
        
        // 1. Evaluate Routing Subject based on parse validity
        //    Invalid envelopes are dead-lettered and explicitly kept out of the unified stream.
        if (env.provenance.parse_status == vocab::ParseStatus::Error) {
            route.subject = AnnexConsumerRouter::build_invalid_subject(profile.adapter_name);
        } else if (AnnexConsumerRouter::is_correction_event(env.linkage.correction_type)) {
            // Divert explicit corrections to their own processing workflow
            route.subject = AnnexConsumerRouter::build_correction_subject(profile.adapter_name);
        } else {
            // Coarse subject-level version routing for clean lifecycle progression
            route.subject = AnnexConsumerRouter::build_lifecycle_subject(profile.adapter_name);
        }

        // 2. Build the Payload-Level Projection
        //    Identifies the exact governed payload contract regardless of routing subject
        route.payload.schema_version = "v1.0.0"; 
        
        // 3. Map Canonical Fields without dragging internal struct layout forward
        route.payload.event_id = std::to_string(env.identity.event_id.value);
        route.payload.instrument_id = env.identity.instrument_id;
        route.payload.account_id = env.identity.account_id;
        route.payload.strategy_id = env.identity.strategy_id;
        route.payload.firm_order_id = env.identity.firm_order_id;

        route.payload.event_type = env.semantics.event_type;
        route.payload.normalized_state = env.semantics.normalized_state;
        route.payload.reason_code = env.semantics.reason_code;
        route.payload.side = env.semantics.side;
        route.payload.order_type = env.semantics.order_type;
        route.payload.time_in_force = env.semantics.time_in_force;
        
        route.payload.quantity = env.semantics.quantity;
        route.payload.price = env.semantics.price;

        route.payload.source_time_nanos = env.time.source_time.epoch_nanos;
        route.payload.venue_time_nanos = env.time.venue_time.epoch_nanos;
        
        route.payload.payload_hash = std::to_string(env.provenance.payload_hash.hash_value);
        route.payload.adapter_name = std::string(env.provenance.adapter_name);
        route.payload.adapter_version = std::string(env.provenance.adapter_version);
        route.payload.parse_status = env.provenance.parse_status;
        
        route.payload.correction_type = env.linkage.correction_type;

        route.payload.venue_native_event_code = std::string(env.extensions.venue_native_event_code);
        route.payload.venue_native_reason_code = std::string(env.extensions.venue_native_reason_code);
        
        for (const auto& tag : env.extensions.tags) {
            route.payload.survival_tags.emplace_back(tag.tag_id, std::string(tag.value));
        }

        return route;
    }
};

} // namespace integration
} // namespace omega
} // namespace quanux
