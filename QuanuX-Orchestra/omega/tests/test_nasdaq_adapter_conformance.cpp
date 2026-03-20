#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/nasdaq/nasdaq_adapter.hpp"
#include "quanux/omega/adapters/nasdaq/stock_directory.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"

using namespace quanux::omega;
using namespace quanux::omega::adapters::nasdaq;
using namespace quanux::omega::integration;

void test_nasdaq_semantic_success() {
    // 1. Explicit Lifecycle Mock (Pre-Market Preload Phase)
    auto& directory = StockDirectoryRegistry::getInstance();
    directory.clear_for_new_trading_day(); 
    directory.declare_locate(42, "AAPL");

    NasdaqIngressMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.message_type = 'A'; // Add order
    msg.stock_locate = __builtin_bswap16(42); // Physically exists in the registry natively

    msg.tracking_number = __builtin_bswap16(1);
    msg.order_reference_number = __builtin_bswap64(12345);
    std::strncpy(msg.stock_symbol, "AAPL    ", 8);
    std::strncpy(msg.mpid, "GSCO", 4);
    msg.side = 'B';
    msg.shares = __builtin_bswap32(100);
    msg.price = __builtin_bswap32(1500000); // $150.0000
    msg.timestamp_nanos = __builtin_bswap64(1000000000);

    core::OmegaEventEnvelope env;
    bool parsed = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env);
    assert(parsed);
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Semantic boundaries hit
    assert(env.identity.event_id.value == 12345);
    assert(env.identity.instrument_id == "AAPL"); // Explicit locate derivation mapped accurately through O(1) Arrays
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
    no_time.stock_locate = __builtin_bswap16(42);
    no_time.order_reference_number = __builtin_bswap64(1234);
    no_time.timestamp_nanos = 0; // SEMANTIC FAILURE

    core::OmegaEventEnvelope env2;
    bool parsed2 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&no_time), sizeof(no_time), env2);
    assert(!parsed2);
    assert(env2.provenance.parse_status == vocab::ParseStatus::Error);
    
    // Unknown state
    NasdaqIngressMock unknown_state{};
    std::memset(&unknown_state, 0, sizeof(unknown_state));
    unknown_state.message_type = 'Z'; // SEMANTIC FAILURE (Unknown)
    unknown_state.stock_locate = __builtin_bswap16(42);
    unknown_state.order_reference_number = __builtin_bswap64(1234);
    unknown_state.timestamp_nanos = __builtin_bswap64(1);

    // Missing/Unmapped StockLocate Validation Test
    NasdaqIngressMock unmapped_locate{};
    std::memset(&unmapped_locate, 0, sizeof(unmapped_locate));
    unmapped_locate.message_type = 'A';
    unmapped_locate.stock_locate = __builtin_bswap16(9999); // UNMAPPED
    unmapped_locate.order_reference_number = __builtin_bswap64(54321);
    unmapped_locate.timestamp_nanos = __builtin_bswap64(1);
    
    core::OmegaEventEnvelope env4;
    bool parsed4 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&unmapped_locate), sizeof(unmapped_locate), env4);
    assert(!parsed4); // Rejected dynamically natively
    assert(env4.provenance.parse_status == vocab::ParseStatus::Error);
    
    // Testing Stale Location Resolution
    auto& directory = StockDirectoryRegistry::getInstance();
    directory.declare_locate(45, "MSFT");
    
    NasdaqIngressMock stale_mock{};
    std::memset(&stale_mock, 0, sizeof(stale_mock));
    stale_mock.message_type = 'A';
    stale_mock.stock_locate = __builtin_bswap16(45); // Valid
    stale_mock.order_reference_number = __builtin_bswap64(9999);
    stale_mock.timestamp_nanos = __builtin_bswap64(1);
    
    core::OmegaEventEnvelope env5;
    bool parsed5 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&stale_mock), sizeof(stale_mock), env5);
    assert(parsed5);
    assert(env5.identity.instrument_id == "MSFT");
    
    // CLEARING the Trading Day
    directory.clear_for_new_trading_day();
    core::OmegaEventEnvelope env6;
    bool parsed6 = NasdaqAdapter::parse_ingress_message(reinterpret_cast<const uint8_t*>(&stale_mock), sizeof(stale_mock), env6);
    assert(!parsed6); // Fails because dictionary was cleanly dropped natively

    std::cout << "[NASDAQ] Directory & Semantic Failure Guards Passed" << std::endl;
}

int main() {
    test_nasdaq_semantic_success();
    test_nasdaq_semantic_failures();
    std::cout << "NASDAQ Conformance complete." << std::endl;
    return 0;
}
