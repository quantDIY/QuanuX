#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/lme/lme_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::lme;
using namespace quanux::omega::integration;

void test_lme_semantic_success() {
    LmeExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.msg_type = 8;
    msg.order_id = 9988776655;
    std::strncpy(msg.instrument_code, "CA  ", 4);
    msg.side = '2'; // Sell
    msg.quantity = 50;
    msg.price = 8500.50; 
    msg.exec_type = '2'; // Full Fill
    msg.transact_time_nanos = 5000000;

    core::OmegaEventEnvelope env;
    bool parsed = LmeAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification 
    assert(env.identity.event_id.value == 9988776655); 
    
    assert(env.identity.instrument_id == "CA");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(std::abs(env.semantics.price - 8500.50) < 0.0001);

    // Provenance bindings
    assert(env.provenance.adapter_name == "LME_MOCK");
    assert(env.extensions.venue_native_event_code == "8");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, LmeAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.LME_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[LME] Clean Lifecycle Routing Passed" << std::endl;
}

void test_lme_semantic_failures() {
    LmeExecutionMock bad_msg{};
    std::memset(&bad_msg, 0, sizeof(bad_msg));
    bad_msg.order_id = 0; // SEMANTIC FAILURE 

    core::OmegaEventEnvelope env1;
    bool parsed1 = LmeAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_msg), sizeof(bad_msg), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, LmeAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.LME_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); 

    std::cout << "[LME] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_lme_semantic_success();
    test_lme_semantic_failures();
    std::cout << "LME Conformance complete." << std::endl;
    return 0;
}
