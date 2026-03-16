#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/nyse/nyse_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::nyse;
using namespace quanux::omega::integration;

void test_nyse_semantic_success() {
    NysePillarMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.msg_type = 0x210; // Execution
    msg.me_order_id = 99999;
    msg.client_order_id = 0; // Using ME order ID is valid
    std::strncpy(msg.symbol, "MSFT       ", 11);
    msg.side = 3; // Sell short
    msg.qty = 50;
    msg.price = 20050000000; // $200.50 (8-decimal int space natively)
    msg.execution_status = '2'; // Filled
    msg.source_time_ns = 555555;

    core::OmegaEventEnvelope env;
    bool parsed = NyseAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic mapping hit
    assert(env.identity.event_id.value == 99999);
    assert(env.identity.instrument_id == "MSFT");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(env.semantics.price == 200.50);

    // Provenance extensions hit
    assert(env.provenance.adapter_name == "NYSE_PILLAR_MOCK");
    assert(env.extensions.venue_native_event_code == "0210");

    // Generic Annex routing tests
    auto route = AnnexPublisher::project_and_route(env, NyseAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.NYSE_PILLAR_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[NYSE] Clean Lifecycle Routing Passed" << std::endl;
}

void test_nyse_semantic_failures() {
    // Missing required identity (Both 0)
    NysePillarMock no_id{};
    std::memset(&no_id, 0, sizeof(no_id));
    no_id.msg_type = 0x200; // New order
    no_id.me_order_id = 0; // SEMANTIC FAILURE
    no_id.client_order_id = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = NyseAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_id), sizeof(no_id), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex dead-letter fallback applies generic deadletter rules to NYSE payloads automatically
    auto route1 = AnnexPublisher::project_and_route(env1, NyseAdapter::get_capability_profile());
    assert(route1.subject == "omega.events.invalid.v1.NYSE_PILLAR_MOCK");

    // Contradictory/Unknown State mapping
    NysePillarMock bad_state{};
    std::memset(&bad_state, 0, sizeof(bad_state));
    bad_state.msg_type = 0x210; 
    bad_state.me_order_id = 123;
    bad_state.source_time_ns = 1;
    bad_state.execution_status = '9'; // SEMANTIC FAILURE (Unknown execution code)

    core::OmegaEventEnvelope env2;
    bool parsed2 = NyseAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_state), sizeof(bad_state), env2);
    assert(!parsed2);
    assert(env2.provenance.parse_status == vocab::ParseStatus::Error);

    std::cout << "[NYSE] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_nyse_semantic_success();
    test_nyse_semantic_failures();
    std::cout << "NYSE Conformance complete." << std::endl;
    return 0;
}
