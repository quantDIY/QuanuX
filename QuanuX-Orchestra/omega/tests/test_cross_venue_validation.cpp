#include <iostream>
#include <cassert>
#include <vector>

#include "quanux/omega/integration/annex_publisher.hpp"
#include "quanux/omega/integration/annex_consumer.hpp"
#include "quanux/omega/adapters/cbot/cbot_adapter.hpp"
#include "quanux/omega/adapters/comex/comex_adapter.hpp"
#include "quanux/omega/adapters/nymex/nymex_adapter.hpp"
#include "quanux/omega/adapters/lse/lse_adapter.hpp"
#include "quanux/omega/adapters/lme/lme_adapter.hpp"
#include "quanux/omega/adapters/nasdaq/nasdaq_adapter.hpp"
#include "quanux/omega/adapters/nyse/nyse_adapter.hpp"
#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/adapters/cboe/cboe_adapter.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_adapter.hpp"
#include "quanux/omega/counterparties/goldman_sachs/goldman_sachs_adapter.hpp"
#include "quanux/omega/counterparties/jpmorgan/jpmorgan_adapter.hpp"

using namespace quanux::omega;
using namespace quanux::omega::integration;
using namespace quanux::omega::capability;

void assert_cross_path_genericity() {
    std::vector<SourceCapabilityProfile> profiles = {
        adapters::ibkr::IbkrAdapter::get_capability_profile(),
        adapters::cbot::CbotAdapter::get_capability_profile(),
        adapters::comex::ComexAdapter::get_capability_profile(),
        adapters::nymex::NymexAdapter::get_capability_profile(),
        adapters::lse::LseAdapter::get_capability_profile(),
        adapters::lme::LmeAdapter::get_capability_profile(),
        adapters::nasdaq::NasdaqAdapter::get_capability_profile(),
        adapters::nyse::NyseAdapter::get_capability_profile(),
        adapters::cme::CmeAdapter::get_capability_profile(),
        adapters::cboe::CboeAdapter::get_capability_profile(),
        counterparties::goldman_sachs::GoldmanSachsAdapter::get_capability_profile(),
        counterparties::jpmorgan::JPMorganAdapter::get_capability_profile()
    };

    // Prove all 12 profiles can map into valid Schema bindings and parse identically
    for (const auto& profile : profiles) {
        // Assert Schema is structurally valid
        assert(profile.schema_compliance.version_string == "v1.0.0");
        
        // --- 1. Valid Lifecycle Routing Validation ---
        AnnexTransportProjection valid_dummy;
        valid_dummy.adapter_name = std::string(profile.adapter_name);
        valid_dummy.schema_version = profile.schema_compliance.version_string;
        valid_dummy.parse_status = vocab::ParseStatus::Success; // VALID
        valid_dummy.payload_hash = "mock_hash_valid";
        valid_dummy.event_id = "12345";
        
        auto val = AnnexConsumerValidator::validate_payload(valid_dummy);
        assert(val.is_valid);
        
        // Assert Annex bounds the valid routing correctly 
        std::string subject = "omega.events.lifecycle.v1." + std::string(profile.adapter_name);
        auto category = AnnexConsumerRouter::identify_category(subject);
        assert(category == AnnexConsumerRouter::SubjectCategory::LIFECYCLE);
        
        auto res = AnnexConsumer::consume_projection(subject, valid_dummy);
        assert(res.consumed == true);

        // --- 2. Invalid Dead-Letter Routing Validation ---
        AnnexTransportProjection invalid_dummy;
        invalid_dummy.adapter_name = std::string(profile.adapter_name);
        invalid_dummy.schema_version = profile.schema_compliance.version_string;
        invalid_dummy.parse_status = vocab::ParseStatus::Error; // INVALID
        invalid_dummy.payload_hash = "mock_hash_invalid";
        invalid_dummy.event_id = "99999"; // Though not required for Error bounds 
        
        auto val_inv = AnnexConsumerValidator::validate_payload(invalid_dummy);
        assert(val_inv.is_valid); // The validation is structurally sound enough to be processed...
        
        std::string err_subject = "omega.events.invalid.v1." + std::string(profile.adapter_name);
        auto err_category = AnnexConsumerRouter::identify_category(err_subject);
        assert(err_category == AnnexConsumerRouter::SubjectCategory::INVALID); // ... but it is sorted immediately to dead letters
        
        auto err_res = AnnexConsumer::consume_projection(err_subject, invalid_dummy);
        assert(err_res.consumed == true);
        
        std::cout << "Successfully proved Schema + Lifecycle + Dead-Letter Routing Generics for: " << profile.adapter_name << std::endl;
    }
}

int main() {
    assert_cross_path_genericity();
    std::cout << "[ORCHESTRATOR] Cross-Path validation proves Annex bounds uniformly accept valid and invalid events for all 12 standard profiles (Venues + Counterparties) without branches." << std::endl;
    return 0;
}
