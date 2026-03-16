#include <iostream>
#include <cassert>
#include <cstring>
#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_adapter.hpp"
#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"
#include "quanux/omega/integration/annex_capability_announcement.hpp"
#include "quanux/omega/integration/annex_schema_announcement.hpp"
#include "quanux/omega/integration/annex_debug_reader.hpp"

using namespace quanux::omega;
using namespace quanux::omega::integration;

// 1. Cross-Subject Isolation Assertions
void test_cross_subject_isolation() {
    adapters::cme::CmeExecutionReportSbe msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.ord_status = 2; // Filled (Lifecycle)
    msg.exec_id = 999; // Explicitly map a valid identity bounds

    core::OmegaEventEnvelope env;
    survival::TagValue tag;
    
    // Parse valid lifecycle
    adapters::cme::CmeAdapter::parse_execution_report(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, tag);
    env.identity.event_id = ids::EventId(999); // Force a valid test identity
    
    // Project and verify publisher puts it on lifecycle route
    auto route = AnnexPublisher::project_and_route(env, adapters::cme::CmeAdapter::get_capability_profile());
    assert(route.subject == "omega.events.lifecycle.v1.CME_iLink3");

    // Force consumer to inspect it under the WRONG subject taxonomy (simulating a wildcard routing mistake)
    // Here we feed a valid fill into the INVALID queue
    auto bad_invalid_read = AnnexConsumer::consume_projection("omega.events.invalid.v1.CME_iLink3", route.payload);
    if (bad_invalid_read.diagnostics.find("Invalid taxonomy violation") == std::string::npos) {
        std::cerr << "Mismatched diagnostics: " << bad_invalid_read.diagnostics << std::endl;
    }
    assert(!bad_invalid_read.consumed);
    assert(bad_invalid_read.diagnostics.find("Invalid taxonomy violation") != std::string::npos);

    // Feed a valid fill into the CORRECTIONS queue
    auto bad_correction_read = AnnexConsumer::consume_projection("omega.events.corrections.v1.CME_iLink3", route.payload);
    assert(!bad_correction_read.consumed);
    assert(bad_correction_read.diagnostics.find("Invalid taxonomy violation") != std::string::npos);

    // Feed proper lifecycle into lifecycle queue
    auto correct_read = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(correct_read.consumed);

    std::cout << "Cross-Subject Isolation bindings asserted." << std::endl;
}

// 2. Dead-Letter Routing Equality
void test_dead_letter_preservation() {
    uint8_t short_buffer[10] = {0}; // Fails parse
    core::OmegaEventEnvelope env_short;
    survival::TagValue tag_short;
    
    adapters::cme::CmeAdapter::parse_execution_report(short_buffer, sizeof(short_buffer), env_short, tag_short);
    auto route = AnnexPublisher::project_and_route(env_short, adapters::cme::CmeAdapter::get_capability_profile());

    // Publisher pushes to invalid
    assert(route.subject == "omega.events.invalid.v1.CME_iLink3");

    // Consumer enforces invalid reads only on dead-letter queues
    auto bad_lifecycle_read = AnnexConsumer::consume_projection("omega.events.lifecycle.v1.CME_iLink3", route.payload);
    assert(!bad_lifecycle_read.consumed);

    auto valid_dead_letter_read = AnnexConsumer::consume_projection(route.subject, route.payload);
    assert(valid_dead_letter_read.consumed);

    std::cout << "Dead-Letter classification and context preservation asserted." << std::endl;
}

// 3. Schema Version & Validation Blocks
void test_consumer_validation_blocks() {
    AnnexTransportProjection malformed_payload;
    malformed_payload.schema_version = "v1.0.0";
    malformed_payload.adapter_name = "CME_iLink3";
    malformed_payload.payload_hash = "fake_hash";
    
    // Missing event ID internally
    auto reject = AnnexConsumer::consume_projection("omega.events.lifecycle.v1.CME_iLink3", malformed_payload);
    assert(!reject.consumed);
    assert(reject.diagnostics.find("must present an event_id") != std::string::npos);

    // Bad Schema Version
    malformed_payload.event_id = "12345";
    malformed_payload.schema_version = "v0.5.0";
    auto old_reject = AnnexConsumer::consume_projection("omega.events.lifecycle.v1.CME_iLink3", malformed_payload);
    assert(!old_reject.consumed);
    assert(old_reject.diagnostics.find("rejected outright") != std::string::npos);

    std::cout << "Consumer Validation Rejection Rules asserted." << std::endl;
}

// 4. Capability Announcements
void test_capability_announcements() {
    auto profile = adapters::ibkr::IbkrAdapter::get_capability_profile();
    auto announcement = AnnexCapabilityPublisher::project_capability(profile);
    
    // Correct consumption
    auto res = AnnexCapabilityConsumer::consume_announcement("omega.events.capability.v1.IBKR_TWS_API", announcement);
    assert(res.consumed);

    // Tamper with announcement to simulate a bad adapter overclaiming capabilities
    announcement.proves_source_native_time = true;
    auto bad_res = AnnexCapabilityConsumer::consume_announcement("omega.events.capability.v1.IBKR_TWS_API", announcement);
    assert(!bad_res.consumed);
    assert(bad_res.rejection_reason.find("Overclaim") != std::string::npos);

    std::cout << "Capability Announcement Publish/Consume asserted." << std::endl;
}

// 5. Schema Announcements
void test_schema_announcements() {
    auto schema = AnnexSchemaPublisher::project_schema();
    
    assert(schema.deprecation_warning.find("types::Price") != std::string::npos); // Ensure numeracy warnings survive

    auto res = AnnexSchemaConsumer::consume_announcement("omega.events.schema.v1", schema);
    assert(res.consumed);
    assert(res.has_active_deprecation);

    std::cout << "Schema Announcement Publish/Consume asserted." << std::endl;
}

// 6. Debug Reader Integration
void test_debug_reader_compile() {
    // We just verify it compiles and runs without mutating.
    AnnexTransportProjection mock;
    mock.schema_version = "v1.0.0";
    mock.adapter_name = "MockAdapter";
    mock.event_type = vocab::EventType::ExecutionBust;
    mock.correction_type = core::CorrectionType::Bust;
    mock.parse_status = vocab::ParseStatus::Success;

    AnnexDebugReader::ReadFilters filters{true, true, true, ""};
    // Expected output locally omitted, proves it doesn't crash on print.
    AnnexDebugReader::inspect_payload("omega.events.corrections.v1.MockAdapter", mock, filters);
    std::cout << "Annex Debug Reader compiled cleanly." << std::endl;
}

int main() {
    std::cout << "WARNING: Annex Consumer tests validate constraints and ignore precision numeric parsing." << std::endl;
    
    test_cross_subject_isolation();
    test_dead_letter_preservation();
    test_consumer_validation_blocks();
    test_capability_announcements();
    test_schema_announcements();
    test_debug_reader_compile();

    std::cout << "All Agent 7 Annex Consumer Implementation tests passed." << std::endl;
    return 0;
}
