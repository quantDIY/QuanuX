#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/counterparties/goldman_sachs/goldman_sachs_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::counterparties::goldman_sachs;
using namespace quanux::omega::integration;

void test_gs_clean_routing() {
    GoldmanSachsIngressMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.msg_type = 8; 
    std::strncpy(msg.client_order_id, "GS_ORDER_001           ", 24);
    std::strncpy(msg.routing_strategy, "SIGMA_X         ", 16);
    msg.side = 'B'; 
    msg.fill_qty = 1000;
    msg.fill_price = 45.50; 
    msg.exec_type = 'F'; // Full Fill
    msg.route_transit_time_nanos = 8000000;

    core::OmegaEventEnvelope env;
    bool parsed = GoldmanSachsAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Identity verification (Counterparty distinct from Venue/Routing)
    assert(env.identity.event_id.value != 0); 
    assert(env.identity.counterparty_id == "GS");
    assert(env.identity.route_id == "SIGMA_X");

    // Semantic verifications
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(env.time.adapter_time.epoch_nanos == 8000000);

    // Generic Annex Routing Test
    auto route = AnnexPublisher::project_and_route(env, GoldmanSachsAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.GOLDMAN_SACHS_PATH");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[Goldman Sachs] Counterparty Identity + Lifecycle Routing Passed" << std::endl;
}

void test_gs_failures() {
    GoldmanSachsIngressMock bad_msg{};
    std::memset(&bad_msg, 0, sizeof(bad_msg));
    bad_msg.route_transit_time_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = GoldmanSachsAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_msg), sizeof(bad_msg), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    auto route1 = AnnexPublisher::project_and_route(env1, GoldmanSachsAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.GOLDMAN_SACHS_PATH");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); 

    std::cout << "[Goldman Sachs] Ambiguous Identity/Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_gs_clean_routing();
    test_gs_failures();
    std::cout << "GS Conformance complete." << std::endl;
    return 0;
}
