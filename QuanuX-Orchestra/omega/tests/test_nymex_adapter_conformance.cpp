#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/nymex/nymex_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::nymex;
using namespace quanux::omega::integration;

void test_nymex_semantic_success() {
    NymexExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.transaction_type = 5;
    std::strncpy(msg.trade_id, "NYMEX-TRADE-123", 15);
    std::strncpy(msg.contract_symbol, "CL  ", 4);
    msg.buy_sell = 'B'; 
    msg.quantity = 25;
    msg.execution_price = 75.50; // Native double 
    msg.execution_status = 0; // Accepted
    msg.transact_time_nanos = 3000000;

    core::OmegaEventEnvelope env;
    bool parsed = NymexAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification (Hash Identity check)
    assert(env.identity.event_id.value != 0); 
    assert(env.identity.venue_order_id.find("NYMEX") != std::string::npos); // String backing hit
    
    assert(env.identity.instrument_id == "CL");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(std::abs(env.semantics.price - 75.50) < 0.0001);

    // Provenance bindings
    assert(env.provenance.adapter_name == "NYMEX_MOCK");
    assert(env.extensions.venue_native_event_code == "5");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, NymexAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.NYMEX_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[NYMEX] Clean Lifecycle Routing Passed" << std::endl;
}

void test_nymex_semantic_failures() {
    NymexExecutionMock bad_id{};
    std::memset(&bad_id, 0, sizeof(bad_id));
    bad_id.trade_id[0] = '\0'; // SEMANTIC FAILURE (No String Identity)
    bad_id.transact_time_nanos = 3000000;

    core::OmegaEventEnvelope env1;
    bool parsed1 = NymexAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_id), sizeof(bad_id), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, NymexAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.NYMEX_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); 

    std::cout << "[NYMEX] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_nymex_semantic_success();
    test_nymex_semantic_failures();
    std::cout << "NYMEX Conformance complete." << std::endl;
    return 0;
}
