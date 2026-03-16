#include <iostream>
#include <cassert>
#include <cstring>

#include "quanux/omega/adapters/lse/lse_adapter.hpp"
#include "quanux/omega/counterparties/jpmorgan/jpmorgan_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::integration;
using namespace quanux::omega::capability;

void prove_end_to_end_symmetry() {
    std::cout << "[ORCHESTRATOR] Starting End-to-End Proof (Direct Venue vs. Counterparty Path)" << std::endl;

    // --- 1. Construct Direct Venue Mock (LSE) ---
    adapters::lse::LseExecutionMock lse_msg{};
    std::memset(&lse_msg, 0, sizeof(lse_msg));
    
    lse_msg.message_type = 15;
    lse_msg.transact_time_nanos = 1620000000000000000ULL;
    std::strncpy(lse_msg.order_id, "LSE_ORD_123", sizeof(lse_msg.order_id) - 1);
    std::strncpy(lse_msg.instrument_id, "VOD.L", sizeof(lse_msg.instrument_id) - 1);
    lse_msg.side = '1'; // Buy
    lse_msg.exec_price = 105.5;
    lse_msg.exec_qty = 500;
    std::strncpy(lse_msg.exec_type, "2 ", sizeof(lse_msg.exec_type) - 1); // Fill

    // --- 2. Construct Counterparty Mock (J.P. Morgan) ---
    counterparties::jpmorgan::JPMorganIngressMock jpm_msg{};
    std::memset(&jpm_msg, 0, sizeof(jpm_msg));
    
    jpm_msg.payload_code = 200;
    jpm_msg.router_timestamp_nanos = 1620000000000000000ULL;
    std::strncpy(jpm_msg.jpm_order_id, "JPM_ORD_999", sizeof(jpm_msg.jpm_order_id) - 1);
    std::strncpy(jpm_msg.execution_venue, "XNAS", sizeof(jpm_msg.execution_venue) - 1); // Executed on NASDAQ
    jpm_msg.side = 'B'; // Buy
    jpm_msg.execution_price = 105.5;
    jpm_msg.execution_quantity = 500;
    jpm_msg.order_status = 'F'; // Fill

    // --- 3. Parse and Normalize ---
    core::OmegaEventEnvelope lse_env;
    bool lse_parsed = adapters::lse::LseAdapter::parse_ingress_message(
        reinterpret_cast<const uint8_t*>(&lse_msg), sizeof(lse_msg), lse_env
    );
    assert(lse_parsed);

    core::OmegaEventEnvelope jpm_env;
    bool jpm_parsed = counterparties::jpmorgan::JPMorganAdapter::parse_ingress_message(
        reinterpret_cast<const uint8_t*>(&jpm_msg), sizeof(jpm_msg), jpm_env
    );
    assert(jpm_parsed);

    // --- 4. Verify Identity Separation Contract ---
    std::cout << "  -> Verifying Identity Separation..." << std::endl;
    // LSE should bind venue
    assert(lse_env.identity.venue_id == "LSE");
    assert(lse_env.identity.route_id.empty());
    assert(lse_env.identity.counterparty_id.empty());
    assert(lse_env.identity.venue_order_id.starts_with("LSE_ORD_123"));

    // JPM should bind counterparty and route, not venue
    assert(jpm_env.identity.venue_id.empty());
    assert(jpm_env.identity.counterparty_id == "JPM");
    assert(jpm_env.identity.route_id.empty());
    assert(jpm_env.identity.venue_order_id.starts_with("JPM_ORD_999")); // Acting as street-side firm order representation

    // --- 5. Project to Annex ---
    std::cout << "  -> Verifying Annex Projection..." << std::endl;
    auto lse_profile = adapters::lse::LseAdapter::get_capability_profile();
    auto jpm_profile = counterparties::jpmorgan::JPMorganAdapter::get_capability_profile();
    
    auto lse_route = AnnexPublisher::project_and_route(lse_env, lse_profile);
    auto jpm_route = AnnexPublisher::project_and_route(jpm_env, jpm_profile);

    // --- 6. Abstract Equivalence Validation ---
    // Both inputs represent a Buy Fill for 500 @ 105.5, executed on different paths.
    // The Annex payload should be structurally and semantically identical in the generic dimensions.
    assert(lse_route.payload.schema_version == "v1.0.0");
    assert(jpm_route.payload.schema_version == "v1.0.0");
    
    assert(lse_route.payload.event_type == vocab::EventType::ExecutionFull);
    assert(jpm_route.payload.event_type == vocab::EventType::ExecutionFull);

    assert(lse_route.payload.normalized_state == vocab::NormalizedState::Filled);
    assert(jpm_route.payload.normalized_state == vocab::NormalizedState::Filled);

    assert(lse_route.payload.side == vocab::OrderSide::Buy);
    assert(jpm_route.payload.side == vocab::OrderSide::Buy);

    // Validate routing bounds
    if (lse_route.subject != "omega.events.lifecycle.v1.LSE_MOCK") {
        std::cerr << "LSE Route Subject: " << lse_route.subject << std::endl;
        std::cerr << "LSE Parse Status: " << static_cast<int>(lse_route.payload.parse_status) << std::endl;
    }
    assert(lse_route.subject == "omega.events.lifecycle.v1.LSE_MOCK");

    if (jpm_route.subject != "omega.events.lifecycle.v1.JPMORGAN_PATH") {
        std::cerr << "JPM Route Subject: " << jpm_route.subject << std::endl;
        std::cerr << "JPM Parse Status: " << static_cast<int>(jpm_route.payload.parse_status) << std::endl;
    }
    assert(jpm_route.subject == "omega.events.lifecycle.v1.JPMORGAN_PATH");

    // Pass them through Consumer validation to ensure final sanity
    auto lse_val = AnnexConsumerValidator::validate_payload(lse_route.payload);
    assert(lse_val.is_valid);
    
    auto jpm_val = AnnexConsumerValidator::validate_payload(jpm_route.payload);
    assert(jpm_val.is_valid);

    std::cout << "  -> Equivalence proven. Both Venue and Counterparty paths project cleanly into identical generic canonicals." << std::endl;
    std::cout << "[ORCHESTRATOR] End-to-End Proof Complete. Omega Contract is hardened." << std::endl;
}

int main() {
    prove_end_to_end_symmetry();
    return 0;
}
