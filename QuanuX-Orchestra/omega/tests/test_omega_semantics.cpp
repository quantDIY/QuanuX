#include <iostream>
#include <cassert>
#include <type_traits>
#include <span>

#include "quanux/omega/omega_core/omega_event_envelope.hpp"
#include "quanux/omega/omega_core/omega_event_semantics.hpp"
#include "quanux/omega/omega_core/omega_event_provenance.hpp"
#include "quanux/omega/omega_core/omega_event_extensions.hpp"

using namespace quanux::omega;

void test_zero_copy_and_layout() {
    // Identity must be standard layout and trivially copyable to ensure it's
    // just a collection of views.
    static_assert(std::is_standard_layout_v<core::OmegaEventIdentity>, "Identity must be standard layout");
    static_assert(std::is_trivially_copyable_v<core::OmegaEventIdentity>, "Identity must be trivially copyable");

    // Extensions and Provenance must be standard layout
    static_assert(std::is_standard_layout_v<core::OmegaEventExtensions>, "Extensions must be standard layout");
    static_assert(std::is_standard_layout_v<core::OmegaEventProvenance>, "Provenance must be standard layout");

    // Ensure there are no hidden std::string heap allocations
    static_assert(sizeof(core::OmegaEventExtensions) <= 64, "Extensions struct exceeds expected size constraint");
    static_assert(sizeof(core::OmegaEventProvenance) <= 128, "Provenance struct exceeds expected size constraint");
}

void test_envelope_construction() {
    core::OmegaEventEnvelope env;

    // 1. Semantics Presence
    env.semantics.event_type = vocab::EventType::OrderAcknowledged;
    env.semantics.side = vocab::OrderSide::Buy;
    env.semantics.order_type = vocab::OrderType::Limit;
    env.semantics.quantity = 100.0;
    env.semantics.price = 1500.50;

    assert(env.semantics.side == vocab::OrderSide::Buy);
    assert(env.semantics.quantity == 100.0);

    // 2. Provenance Presence
    env.provenance.adapter_name = "CME_iLink3";
    env.provenance.adapter_version = "v2.1";
    env.provenance.parse_status = vocab::ParseStatus::Success;

    std::string_view warnings[] = {"Missing Tag 97", "Non-standard price decimal"};
    env.provenance.parse_warnings = std::span<const std::string_view>{warnings};

    assert(env.provenance.adapter_name == "CME_iLink3");
    assert(env.provenance.parse_warnings.size() == 2);

    // 3. Extensions Lifetime/View Safety
    survival::TagValue tag1{97, "Y"};
    survival::TagValue tag2{204, "0"};
    survival::TagValue tag_buffer[] = {tag1, tag2};

    env.extensions.venue_native_event_code = "A";
    env.extensions.tags = std::span<const survival::TagValue>{tag_buffer};

    assert(env.extensions.venue_native_event_code == "A");
    assert(env.extensions.tags.size() == 2);
    assert(env.extensions.tags[0].tag_id == 97);

    // 4. Identity field access
    env.identity.account_id = "ACC123";
    env.identity.instrument_id = "ESM4";
    env.identity.firm_order_id = "ORDER-999";
    
    assert(env.identity.instrument_id == "ESM4");

    // 5. Time precedence field presence
    env.time.source_time = time::TimestampNode{};
    env.time.venue_time = time::TimestampNode{};
    env.time.receive_time = time::TimestampNode{};
    env.time.adapter_time = time::TimestampNode{};   // explicit source decoding time
    env.time.normalization_time = time::TimestampNode{};

    std::cout << "All Omega Event semantic tests passed perfectly." << std::endl;
}

int main() {
    test_zero_copy_and_layout();
    test_envelope_construction();
    return 0;
}
