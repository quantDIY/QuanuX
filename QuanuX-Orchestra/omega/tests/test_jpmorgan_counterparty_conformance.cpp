#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/counterparties/jpmorgan/jpmorgan_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::counterparties::jpmorgan;
using namespace quanux::omega::integration;

void test_jpm_clean_routing() {
    JPMorganIngressMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.payload_code = 200; 
    std::strncpy(msg.jpm_order_id, "JPM_ORD_999                     ", 32);
    std::strncpy(msg.execution_venue, "XNYS    ", 8);
    msg.side = 'S'; 
    msg.execution_quantity = 50;
    msg.execution_price = 120.00; 
    msg.order_status = 'P'; // Partial Fill
    msg.router_timestamp_nanos = 9000000;

    core::OmegaEventEnvelope env;
    bool parsed = JPMorganAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Identity verification (Counterparty vs Venue Protocol tracking)
    assert(env.identity.event_id.value != 0); 
    assert(env.identity.counterparty_id == "JPM");
    assert(env.provenance.source_protocol == "XNYS");

    // Semantic verifications
    assert(env.semantics.event_type == vocab::EventType::ExecutionPartial);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(env.time.adapter_time.epoch_nanos == 9000000);

    // Generic Annex Routing Test
    auto route = AnnexPublisher::project_and_route(env, JPMorganAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.JPMORGAN_PATH");
    auto read_res = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(read_res.consumed);
    
    std::cout << "[JPMorgan] Counterparty Identity + Venue Transparency Routing Passed" << std::endl;
}

int main() {
    test_jpm_clean_routing();
    std::cout << "JPM Conformance complete." << std::endl;
    return 0;
}
