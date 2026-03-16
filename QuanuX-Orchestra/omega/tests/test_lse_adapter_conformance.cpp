#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/lse/lse_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::lse;
using namespace quanux::omega::integration;

void test_lse_semantic_success() {
    LseExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.message_type = 15;
    std::strncpy(msg.order_id, "LSE123456789        ", 20);
    std::strncpy(msg.instrument_id, "VOD.L       ", 12);
    msg.side = '1'; // Buy
    msg.exec_qty = 1000;
    msg.exec_price = 73.25; 
    std::strncpy(msg.exec_type, "1 ", 2); // Partial Fill
    msg.transact_time_nanos = 4000000;

    core::OmegaEventEnvelope env;
    bool parsed = LseAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification 
    assert(env.identity.event_id.value != 0); 
    assert(env.identity.venue_order_id.find("LSE") != std::string::npos);
    
    assert(env.identity.instrument_id == "VOD.L");
    assert(env.semantics.event_type == vocab::EventType::ExecutionPartial);
    assert(env.semantics.normalized_state == vocab::NormalizedState::PartiallyFilled);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(std::abs(env.semantics.price - 73.25) < 0.0001);

    // Provenance bindings
    assert(env.provenance.adapter_name == "LSE_MOCK");
    assert(env.extensions.venue_native_event_code == "15");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, LseAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.LSE_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[LSE] Clean Lifecycle Routing Passed" << std::endl;
}

void test_lse_semantic_failures() {
    LseExecutionMock bad_msg{};
    std::memset(&bad_msg, 0, sizeof(bad_msg));
    std::strncpy(bad_msg.order_id, "LSE999              ", 20);
    bad_msg.transact_time_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = LseAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_msg), sizeof(bad_msg), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, LseAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.LSE_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); 

    std::cout << "[LSE] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_lse_semantic_success();
    test_lse_semantic_failures();
    std::cout << "LSE Conformance complete." << std::endl;
    return 0;
}
