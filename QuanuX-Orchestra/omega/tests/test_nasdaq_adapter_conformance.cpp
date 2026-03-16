#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/nasdaq/nasdaq_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::nasdaq;
using namespace quanux::omega::integration;

void test_nasdaq_semantic_success() {
    NasdaqIngressMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.message_type = 'A'; // Add order
    msg.order_reference_number = 12345;
    std::strncpy(msg.stock_symbol, "AAPL    ", 8);
    std::strncpy(msg.mpid, "GSCO", 4);
    msg.side = 'B';
    msg.shares = 100;
    msg.price = 1500000; // $150.0000
    msg.timestamp_nanos = 1000000000;

    core::OmegaEventEnvelope env;
    bool parsed = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic boundaries hit
    assert(env.identity.event_id.value == 12345);
    assert(env.identity.instrument_id == "AAPL");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull); // Represents working/added
    assert(env.semantics.normalized_state == vocab::NormalizedState::New);
    assert(env.semantics.price == 150.0);

    // Provenance bindings hit
    assert(env.provenance.adapter_name == "NASDAQ_ITCH_OUCH_MOCK");
    assert(env.extensions.venue_native_event_code == "A");

    // Pass through Generic Annex Routing without customization
    auto route = AnnexPublisher::project_and_route(env, NasdaqAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.NASDAQ_ITCH_OUCH_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[NASDAQ] Clean Lifecycle Routing Passed" << std::endl;
}

void test_nasdaq_semantic_failures() {
    // Missing required identity
    NasdaqIngressMock no_id{};
    std::memset(&no_id, 0, sizeof(no_id));
    no_id.message_type = 'E'; // Execution
    no_id.order_reference_number = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_id), sizeof(no_id), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex catches the dead letter transparently
    auto route1 = AnnexPublisher::project_and_route(env1, NasdaqAdapter::get_capability_profile());
    assert(route1.subject == "omega.events.invalid.v1.NASDAQ_ITCH_OUCH_MOCK");

    // Missing required time
    NasdaqIngressMock no_time{};
    std::memset(&no_time, 0, sizeof(no_time));
    no_time.message_type = 'A'; 
    no_time.order_reference_number = 1234;
    no_time.timestamp_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env2;
    bool parsed2 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_time), sizeof(no_time), env2);
    assert(!parsed2);
    assert(env2.provenance.parse_status == vocab::ParseStatus::Error);
    
    // Unknown state
    NasdaqIngressMock unknown_state{};
    std::memset(&unknown_state, 0, sizeof(unknown_state));
    unknown_state.message_type = 'Z'; // SEMANTIC FAILURE (Unknown)
    unknown_state.order_reference_number = 1234;
    unknown_state.timestamp_nanos = 1;

    core::OmegaEventEnvelope env3;
    bool parsed3 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&unknown_state), sizeof(unknown_state), env3);
    assert(!parsed3);
    assert(env3.provenance.parse_status == vocab::ParseStatus::Error);

    std::cout << "[NASDAQ] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_nasdaq_semantic_success();
    test_nasdaq_semantic_failures();
    std::cout << "NASDAQ Conformance complete." << std::endl;
    return 0;
}
