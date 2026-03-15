#include <iostream>
#include <cassert>
#include <cstring>
#include <span>

#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"

using namespace quanux::omega;

// NOTE: The numeric fields in these tests interact with Quantity and Price 
// aliases which currently point to 'double'. 
// These double fields are TEMPORARY PLACEHOLDERS. 
// These assertions are written to verify semantic translation without baking 
// brittle floating-point equality deeply into the architecture.

void test_cme_order_ack() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1000;
    msg.order_id = 999123;
    msg.exec_id = 555000;
    std::strncpy(msg.cl_ord_id, "CLIENT-ACK-1", sizeof(msg.cl_ord_id));
    msg.transact_time = 1700000000123456789ULL;
    msg.template_id = 15; // Arbitrary template
    msg.side = 1; // Buy
    msg.ord_status = 0; // New
    msg.md_error_code = 0;
    msg.unmapped_native_code = 0;
    msg.order_qty_placeholder = 50.0;
    msg.price_placeholder = 0.0; // Market order

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    bool parsed = adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(parsed);

    // 1. Semantic Translation Presence
    assert(env.semantics.event_type == vocab::EventType::OrderAcknowledged);
    assert(env.semantics.normalized_state == vocab::NormalizedState::New);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    
    // Numeric abstraction check (using tolerance or loose equality)
    assert(env.semantics.quantity > 49.9 && env.semantics.quantity < 50.1); 

    // 2. Identity Extraction
    assert(env.identity.event_id.value == 1000);
    assert(env.identity.client_order_id == "CLIENT-ACK-1");

    // 3. Time Precedence
    assert(env.time.source_time.epoch_nanos == 1700000000123456789ULL);
    assert(env.time.source_time.precedence == time::TimestampPrecedence::SourceNative);
    assert(env.time.adapter_time.epoch_nanos > 0); // Adapter sets decode time

    // 4. Traceability & Lineage
    assert(env.provenance.adapter_name == "CME_iLink3");
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    assert(env.provenance.raw_evidence.data != nullptr);
    assert(env.provenance.raw_evidence.size == sizeof(msg));
    assert(env.provenance.payload_hash.hash_value != 0); // Hash computed

    std::cout << "Order Ack test passed." << std::endl;
}

void test_cme_partial_fill_with_survival() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1001;
    msg.order_id = 999123;
    msg.exec_id = 555001;
    std::strncpy(msg.cl_ord_id, "CLIENT-FILL-1", sizeof(msg.cl_ord_id));
    msg.transact_time = 1700000005000000000ULL;
    msg.template_id = 16; 
    msg.side = 2; // Sell
    msg.ord_status = 1; // PartiallyFilled
    msg.md_error_code = 0;
    msg.unmapped_native_code = 404; // Important exchange-specific instruction
    msg.order_qty_placeholder = 25.0; // P-Fill qty
    msg.price_placeholder = 4500.25;

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    bool parsed = adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(parsed);

    // 1. Semantic Translation
    assert(env.semantics.event_type == vocab::EventType::ExecutionPartial);
    assert(env.semantics.normalized_state == vocab::NormalizedState::PartiallyFilled);
    
    // Numeric abstraction check
    assert(env.semantics.price > 4500.20 && env.semantics.price < 4500.30); 

    // 2. Survival Extraction (Extensions)
    // The native event code should be exactly the bytes of '16'
    assert(reinterpret_cast<const uint8_t*>(env.extensions.venue_native_event_code.data())[0] == 16);
    
    // The unmapped code 404 should survive into tags
    assert(env.extensions.tags.size() == 1);
    assert(env.extensions.tags[0].tag_id == 9999);
    assert(*reinterpret_cast<const uint16_t*>(env.extensions.tags[0].value.data()) == 404);

    std::cout << "Partial Fill with Survival test passed." << std::endl;
}

void test_cme_bust_correction() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1002;
    msg.ord_status = 5; // Bust

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    // 1. Linkage Flagging
    assert(env.linkage.correction_type == core::CorrectionType::Bust);

    std::cout << "Bust/Correction test passed." << std::endl;
}

void test_cme_cancel_replace() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1003;
    msg.ord_status = 6; // Correction / Cancel-Replace
    msg.md_error_code = 0;

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(env.linkage.correction_type == core::CorrectionType::PriceCorrection);
    std::cout << "Cancel/Replace Correction test passed." << std::endl;
}

void test_cme_replay_recovery() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1004;
    msg.ord_status = 0; // New
    msg.template_id = 99; // Assume some replay template id

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );
    
    // Simulate a check that replay logic is handled (currently just generic ack)
    assert(env.semantics.event_type == vocab::EventType::OrderAcknowledged);
    assert(reinterpret_cast<const uint8_t*>(env.extensions.venue_native_event_code.data())[0] == 99);
    std::cout << "Replay/Recovery parsing test passed." << std::endl;
}

void test_cme_extension_heavy() {
    adapters::cme::CmeExecutionReportSbe msg{};
    msg.sequence_number = 1005;
    msg.template_id = 200; // Unknown template
    msg.unmapped_native_code = 65535; // Max unmapped code
    
    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );
    
    assert(env.extensions.tags.size() == 1);
    assert(env.extensions.tags[0].tag_id == 9999);
    assert(*reinterpret_cast<const uint16_t*>(env.extensions.tags[0].value.data()) == 65535);
    std::cout << "Extension-heavy test passed." << std::endl;
}

int main() {
    std::cout << "WARNING: tests checking env.semantics.quantity and price do so fully aware "
              << "that the current 'double' types are temporary abstractions to be hardened later." << std::endl;
    
    test_cme_order_ack();
    test_cme_partial_fill_with_survival();
    test_cme_bust_correction();
    test_cme_cancel_replace();
    test_cme_replay_recovery();
    test_cme_extension_heavy();
    
    std::cout << "All Agent 3 Conformance tests passed." << std::endl;
    return 0;
}
