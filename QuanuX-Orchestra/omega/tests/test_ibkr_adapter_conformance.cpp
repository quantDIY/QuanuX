#include <iostream>
#include <cassert>
#include <cstring>
#include <span>

#include "quanux/omega/adapters/ibkr/ibkr_adapter.hpp"
#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"
#include "quanux/omega/omega_core/omega_event_envelope.hpp"

using namespace quanux::omega;

// Mocks the Order Acknowledged scenario
void test_ibkr_order_ack() {
    adapters::ibkr::IbkrExecutionReportMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.reqId = 1001;
    std::strncpy(msg.execId, "IBKR-ACK-1", sizeof(msg.execId));
    std::strncpy(msg.status, "Submitted", sizeof(msg.status));
    std::strncpy(msg.side, "BOT", sizeof(msg.side));
    msg.shares = 100.0;
    msg.price = 0.0; // market
    
    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    bool parsed = adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(parsed);
    
    // Provenance Traceability
    assert(env.provenance.parse_status == vocab::ParseStatus::Success);
    assert(env.provenance.adapter_name == "IBKR_TWS_API");
    assert(env.provenance.raw_evidence.data != nullptr);
    assert(env.provenance.payload_hash.hash_value != 0);

    // Semantics
    assert(env.semantics.event_type == vocab::EventType::OrderAcknowledged);
    assert(env.semantics.normalized_state == vocab::NormalizedState::New);
    assert(env.semantics.side == vocab::OrderSide::Buy);
    
    std::cout << "Order Ack test passed." << std::endl;
}

// Mocks a Full Fill with unmapped parameter extension survival
void test_ibkr_fill_with_survival() {
    adapters::ibkr::IbkrExecutionReportMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.reqId = 1002;
    std::strncpy(msg.execId, "IBKR-FILL-1", sizeof(msg.execId));
    std::strncpy(msg.status, "Filled", sizeof(msg.status));
    std::strncpy(msg.side, "SLD", sizeof(msg.side));
    msg.shares = 50.0;
    msg.avgPrice = 4500.50;
    msg.errorCode = 999; // Assume some info code we want to survive

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    bool parsed = adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(parsed);
    assert(env.semantics.event_type == vocab::EventType::ExecutionFull);
    assert(env.semantics.side == vocab::OrderSide::Sell);
    assert(env.semantics.last_px > 4500.49 && env.semantics.last_px < 4500.51);

    // Survival
    assert(env.extensions.venue_native_event_code == "Filled");
    assert(env.extensions.tags.size() == 1);
    assert(*reinterpret_cast<const int32_t*>(env.extensions.tags[0].value.data()) == 999);

    std::cout << "Full Fill and Survival test passed." << std::endl;
}

// Mocks a Trade Cancelled (Bust)
void test_ibkr_bust() {
    adapters::ibkr::IbkrExecutionReportMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    std::strncpy(msg.status, "TradeCancelled", sizeof(msg.status));

    core::OmegaEventEnvelope env;
    survival::TagValue survival_tag;
    adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env, survival_tag
    );

    assert(env.linkage.correction_type == core::CorrectionType::Bust);
    std::cout << "Bust Mapping test passed." << std::endl;
}

// 4. Semantic Equivalence across IBKR and CME
void test_semantic_equivalence() {
    // Both adapters processing an identical business event (Order Rejected by Exchange due to Invalid Symbol)
    
    // CME Source
    adapters::cme::CmeExecutionReportSbe cme_msg{};
    std::memset(&cme_msg, 0, sizeof(cme_msg));
    cme_msg.side = 1; // Buy
    cme_msg.ord_status = 8; // Rejected
    cme_msg.md_error_code = 100; // Invalid Symbol in our CME reason mapper
    
    // IBKR Source
    adapters::ibkr::IbkrExecutionReportMock ibkr_msg{};
    std::memset(&ibkr_msg, 0, sizeof(ibkr_msg));
    std::strncpy(ibkr_msg.side, "BOT", sizeof(ibkr_msg.side));
    std::strncpy(ibkr_msg.status, "Inactive", sizeof(ibkr_msg.status));
    ibkr_msg.errorCode = 200; // Invalid Symbol in our IBKR reason mapper

    core::OmegaEventEnvelope cme_env, ibkr_env;
    survival::TagValue cme_tag, ibkr_tag;

    adapters::cme::CmeAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&cme_msg), sizeof(cme_msg), cme_env, cme_tag
    );
    
    adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&ibkr_msg), sizeof(ibkr_msg), ibkr_env, ibkr_tag
    );

    // Assert equivalence in business normalization
    assert(cme_env.semantics.event_type == ibkr_env.semantics.event_type);
    assert(cme_env.semantics.normalized_state == ibkr_env.semantics.normalized_state);
    assert(cme_env.semantics.reason_code == ibkr_env.semantics.reason_code);
    assert(cme_env.semantics.side == ibkr_env.semantics.side);
    
    // Specifically test they mapped correctly to the same enum
    assert(cme_env.semantics.reason_code == vocab::NormalizedReasonCode::InvalidSymbol);
    assert(cme_env.semantics.side == vocab::OrderSide::Buy);

    std::cout << "Cross-Adapter Semantic Equivalence test passed." << std::endl;
}

// 5. Negative Path: Malformed and partial parse handling
void test_ibkr_negative_path() {
    // 1. Malformed payload
    uint8_t short_buffer[10] = {0}; // Way too short for IbkrExecutionReportMock
    core::OmegaEventEnvelope env_short;
    survival::TagValue tag_short;
    
    bool parsed = adapters::ibkr::IbkrAdapter::parse_execution_report(
        short_buffer, sizeof(short_buffer), env_short, tag_short
    );

    assert(!parsed);
    assert(env_short.provenance.parse_status == vocab::ParseStatus::Error);
    assert(env_short.provenance.adapter_name == "IBKR_TWS_API"); // Even on fail, adapter identity is known

    // 2. completely unknown status / unsupported combo
    adapters::ibkr::IbkrExecutionReportMock msg{};
    std::memset(&msg, 0, sizeof(msg));
    std::strncpy(msg.status, "Mysterious", sizeof(msg.status) - 1);
    std::strncpy(msg.whyHeld, "Some random text that is unmapped", sizeof(msg.whyHeld) - 1);
    msg.errorCode = 99999;
    
    core::OmegaEventEnvelope env_unknown;
    survival::TagValue tag_unknown;
    parsed = adapters::ibkr::IbkrAdapter::parse_execution_report(
        reinterpret_cast<const uint8_t*>(&msg), sizeof(msg), env_unknown, tag_unknown
    );
    
    assert(parsed);
    assert(env_unknown.provenance.parse_status == vocab::ParseStatus::Success);
    
    // Core explicitly does not fake certainty:
    assert(env_unknown.semantics.event_type == vocab::EventType::Unknown);
    assert(env_unknown.semantics.normalized_state == vocab::NormalizedState::Unknown);
    
    // Survival still securely holds the missing data
    assert(env_unknown.extensions.venue_native_event_code == "Mysterious");
    assert(env_unknown.extensions.venue_native_reason_code == "Some random text that is unmapped");
    assert(env_unknown.extensions.tags.size() == 1);

    std::cout << "Negative Path Parse & Survival test passed." << std::endl;
}

int main() {
    std::cout << "WARNING: tests checking env.semantics.quantity and price do so fully aware "
              << "that the current 'double' types are temporary abstractions to be hardened later." << std::endl;

    test_ibkr_order_ack();
    test_ibkr_fill_with_survival();
    test_ibkr_bust();
    test_semantic_equivalence();
    test_ibkr_negative_path();

    std::cout << "All Agent 4 IBKR Conformance tests passed." << std::endl;
    return 0;
}
