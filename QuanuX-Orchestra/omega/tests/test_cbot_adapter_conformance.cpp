#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/cbot/cbot_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::cbot;
using namespace quanux::omega::integration;

void test_cbot_semantic_success() {
    CbotExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.msg_type = 1;
    msg.cbot_order_id = 112233;
    std::strncpy(msg.product_group, "ZC  ", 4);
    msg.side = 1; // Buy
    msg.fill_qty = 50;
    msg.fill_price = 45000000000; // $450.00 (8 decimal Native CBOT style)
    msg.status_code = '4'; // Filled
    msg.transact_time_nanos = 1000000;

    core::OmegaEventEnvelope env;
    bool parsed = CbotAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification
    assert(env.identity.event_id.value == 112233);
    assert(env.identity.instrument_id == "ZC");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(env.semantics.price == 450.0);

    // Provenance bindings hit (Phase 8 verification)
    assert(env.provenance.adapter_name == "CBOT_MOCK");
    assert(env.extensions.venue_native_event_code == "1");

    // Pass through Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, CbotAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.CBOT_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[CBOT] Clean Lifecycle Routing Passed" << std::endl;
}

void test_cbot_semantic_failures() {
    // Missing required identity
    CbotExecutionMock no_id{};
    std::memset(&no_id, 0, sizeof(no_id));
    no_id.msg_type = 1; 
    no_id.cbot_order_id = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = CbotAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_id), sizeof(no_id), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, CbotAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; // Fulfill minimum envelope diagnostic requirements
    assert(route1.subject == "omega.events.invalid.v1.CBOT_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); // Invalid consumer safely eats this

    std::cout << "[CBOT] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_cbot_semantic_success();
    test_cbot_semantic_failures();
    std::cout << "CBOT Conformance complete." << std::endl;
    return 0;
}
