#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/cboe/cboe_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::cboe;
using namespace quanux::omega::integration;

void test_cboe_equities_success() {
    CboeExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.message_type = 0x24; // Execution
    std::strncpy(msg.order_id, "BZX_123456          ", 20);
    std::strncpy(msg.symbol, "AAPL        ", 12);
    msg.asset_class = 'E'; // Equity
    std::strncpy(msg.venue_identifier, "BZX ", 4);
    msg.side = '1'; // Buy
    msg.exec_qty = 500;
    msg.exec_price = 150.25; 
    msg.exec_report_type = '2'; // Full Fill
    msg.transact_time_nanos = 6000000;

    core::OmegaEventEnvelope env;
    bool parsed = CboeAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic verification 
    assert(env.identity.event_id.value != 0); 
    assert(env.identity.instrument_id == "AAPL");
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.normalized_state == vocab::NormalizedState::Filled);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(std::abs(env.semantics.price - 150.25) < 0.0001);

    // Provenance bindings
    assert(env.provenance.adapter_name == "CBOE_FAMILY_MOCK");
    assert(env.extensions.venue_native_event_code == "36-CBOE-EQUITIES-BZX");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, CboeAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.CBOE_FAMILY_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[Cboe] Clean Equities Lifecycle Routing Passed" << std::endl;
}

void test_cboe_options_success() {
    CboeExecutionMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.message_type = 0x25; 
    std::strncpy(msg.order_id, "C1_000987           ", 20);
    std::strncpy(msg.symbol, "SPXW        ", 12);
    msg.asset_class = 'O'; // Options
    std::strncpy(msg.venue_identifier, "C1  ", 4);
    msg.side = '5'; // Sell Short
    msg.exec_qty = 10;
    msg.exec_price = 12.50; 
    msg.exec_report_type = '1'; // Partial
    msg.transact_time_nanos = 7000000;

    core::OmegaEventEnvelope env;
    bool parsed = CboeAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    assert(env.identity.instrument_id == "SPXW");
    assert(env.semantics.event_type == vocab::EventType::ExecutionPartial);
    assert(env.semantics.normalized_state == vocab::NormalizedState::PartiallyFilled);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(env.extensions.venue_native_event_code == "37-CBOE-OPTIONS-C1");

    // Generic Annex Routing
    auto route = AnnexPublisher::project_and_route(env, CboeAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.CBOE_FAMILY_MOCK");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[Cboe] Clean Options Lifecycle Routing Passed" << std::endl;
}

void test_cboe_semantic_failures() {
    CboeExecutionMock bad_msg{};
    std::memset(&bad_msg, 0, sizeof(bad_msg));
    std::strncpy(bad_msg.order_id, "CBOEERR             ", 20);
    bad_msg.transact_time_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env1;
    bool parsed1 = CboeAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&bad_msg), sizeof(bad_msg), env1);
    assert(!parsed1);
    assert(env1.provenance.parse_status == vocab::ParseStatus::Error);

    // Verify Annex projection flow routes these implicitly to Dead Letters
    auto route1 = AnnexPublisher::project_and_route(env1, CboeAdapter::get_capability_profile());
    route1.payload.payload_hash = "mock_hash"; 
    assert(route1.subject == "omega.events.invalid.v1.CBOE_FAMILY_MOCK");
    auto dead_res = AnnexConsumer::consume_projection(route1.subject, route1.payload);
    assert(dead_res.consumed); 

    std::cout << "[Cboe] Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_cboe_equities_success();
    test_cboe_options_success();
    test_cboe_semantic_failures();
    std::cout << "Cboe Conformance complete." << std::endl;
    return 0;
}
