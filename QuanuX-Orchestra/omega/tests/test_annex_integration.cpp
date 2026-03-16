#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"

using namespace quanux::omega;
using namespace quanux::omega::integration;

void test_cme_projection() {
    adapters::cme::CmeExecutionReportSbe msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.ord_status = 2; // Filled
    msg.md_error_code = 0; // No error

    core::OmegaEventEnvelope env;
    survival::TagValue tag;
    
    // Parse
    bool parsed = adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, tag
    );
    assert(parsed);

    // Project
    auto profile = adapters::cme::CmeAdapter::get_capability_profile();
    auto route = AnnexPublisher::project_and_route(env, profile);

    // Assert Routing
    assert(route.subject == "omega.events.normalized.v1.CME_iLink3");
    
    // Assert Projection Shape & Content
    assert(route.payload.schema_version == "v1.0.0");
    assert(route.payload.event_type == vocab::EventType::ExecutionFull);
    assert(route.payload.adapter_name == "CME_iLink3");
    
    std::cout << "CME generic projection passed." << std::endl;
}

void test_ibkr_projection() {
    adapters::ibkr::IbkrExecutionReportMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    std::strncpy(msg.status, "Filled", sizeof(msg.status) - 1);
    
    core::OmegaEventEnvelope env;
    survival::TagValue tag;

    // Parse
    bool parsed = adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, tag
    );
    assert(parsed);
    
    // Project
    auto profile = adapters::ibkr::IbkrAdapter::get_capability_profile();
    auto route = AnnexPublisher::project_and_route(env, profile);

    // Assert Routing
    assert(route.subject == "omega.events.normalized.v1.IBKR_TWS_API");
    
    // Assert Projection Shape & Content
    assert(route.payload.schema_version == "v1.0.0");
    assert(route.payload.event_type == vocab::EventType::ExecutionFull);
    assert(route.payload.adapter_name == "IBKR_TWS_API");

    std::cout << "IBKR generic projection passed." << std::endl;
}

// Ensure both CME and IBKR projections serialize through the identical outward contract
void test_compatibility_alignment() {
    adapters::cme::CmeExecutionReportSbe msg_cme{};
    std::memset(&msg_cme, 0, sizeof(msg_cme));
    msg_cme.ord_status = 8; // Rejected
    msg_cme.md_error_code = 100; // Invalid Symbol mock
    core::OmegaEventEnvelope env_cme;
    survival::TagValue tag_cme;
    adapters::cme::CmeAdapter::parse_execution_report(reinterpret_cast<const uint8_t*>(&msg_cme), sizeof(msg_cme), env_cme, tag_cme);
    auto route_cme = AnnexPublisher::project_and_route(env_cme, adapters::cme::CmeAdapter::get_capability_profile());

    adapters::ibkr::IbkrExecutionReportMock msg_ibkr{};
    std::memset(&msg_ibkr, 0, sizeof(msg_ibkr));
    std::strncpy(msg_ibkr.status, "Inactive", sizeof(msg_ibkr.status) - 1);
    msg_ibkr.errorCode = 200; // Invalid Symbol mock
    core::OmegaEventEnvelope env_ibkr;
    survival::TagValue tag_ibkr;
    adapters::ibkr::IbkrAdapter::parse_execution_report(reinterpret_cast<const uint8_t*>(&msg_ibkr), sizeof(msg_ibkr), env_ibkr, tag_ibkr);
    auto route_ibkr = AnnexPublisher::project_and_route(env_ibkr, adapters::ibkr::IbkrAdapter::get_capability_profile());

    // Both should yield the exact same governed schema projection shape
    assert(route_cme.payload.schema_version == route_ibkr.payload.schema_version);
    assert(route_cme.payload.event_type == route_ibkr.payload.event_type); // OrderRejected
    assert(route_cme.payload.reason_code == route_ibkr.payload.reason_code); // InvalidSymbol
    assert(route_cme.payload.event_type == vocab::EventType::OrderRejected);
    assert(route_ibkr.payload.reason_code == vocab::NormalizedReasonCode::InvalidSymbol);

    std::cout << "Annex projection compatibility alignment passed." << std::endl;
}

void test_dead_letter_routing() {
    uint8_t short_buffer[10] = {0}; // Way too short for CME
    core::OmegaEventEnvelope env_short;
    survival::TagValue tag_short;
    
    // Parse will fail, but envelope will have ParseStatus::Error
    bool parsed = adapters::cme::CmeAdapter::parse_execution_report(
        short_buffer, sizeof(short_buffer), env_short, tag_short
    );
    assert(!parsed);

    // Project
    auto profile = adapters::cme::CmeAdapter::get_capability_profile();
    auto route = AnnexPublisher::project_and_route(env_short, profile);

    // Assert Routing is explicitly invalid
    assert(route.subject == "omega.events.invalid.CME_iLink3");
    assert(route.payload.parse_status == vocab::ParseStatus::Error);

    std::cout << "Dead-letter error routing passed." << std::endl;
}

int main() {
    std::cout << "WARNING: Annex tests validating external projections rely on temporary numerics." << std::endl;
    
    test_cme_projection();
    test_ibkr_projection();
    test_compatibility_alignment();
    test_dead_letter_routing();
    
    std::cout << "All Agent 6 Annex Integration tests passed." << std::endl;
    return 0;
}
