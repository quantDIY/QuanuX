#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/comex/comex_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::comex;
using namespace quanux::omega::integration;

void test_comex_semantic_success() {
    ComexMatchReport msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.msg_type = 2;
    msg.comex_order_id = 445566;
    std::strncpy(msg.product_group, "GC  ", 4);
    msg.side = 'S'; // Sell
    msg.qty = 10;
    msg.price = 195000000000; // $1,950 
    msg.order_state = 'F'; // Filled
    msg.transact_time_nanos = 2000000;

    core::OmegaEventEnvelope env;
    bool parsed = ComexAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification
    assert(env.identity.event_id.value == 445566);
    assert(env.identity.instrument_id == "GC");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(env.semantics.price == 1950.0);

    // Provenance bindings
    assert(env.provenance.adapter_name == "COMEX_MOCK");
    assert(env.extensions.venue_native_event_code == "2");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, ComexAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.COMEX_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[COMEX] Clean Lifecycle Routing Passed" << std::endl;
}

void test_comex_semantic_failures() {
    ComexMatchReport no_time{};
    std::memset(&no_time, 0, sizeof(no_time));
    no_time.comex_order_id = 999;
    no_time.transact_time_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = ComexAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_time), sizeof(no_time), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, ComexAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.COMEX_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); // Invalid consumer safely eats this

    std::cout << "[COMEX] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_comex_semantic_success();
    test_comex_semantic_failures();
    std::cout << "COMEX Conformance complete." << std::endl;
    return 0;
}
