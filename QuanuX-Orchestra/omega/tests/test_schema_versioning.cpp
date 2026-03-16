#include <iostream>
#include <cassert>
#include "quanux/omega/omega_capability/schema_versioning.hpp"

using namespace quanux::omega::capability;

void test_schema_minimums() {
    // Current is 1.0.0
    OmegaSchemaVersion target_future{2, 0, 0};
    OmegaSchemaVersion target_past{0, 9, 0};
    OmegaSchemaVersion target_current{1, 0, 0};

    // Asking for a future version we don't support yet
    assert(SchemaVersioning::check_schema_compatibility(target_future) == SchemaCompatibilityStatus::REJECTED_OUTRIGHT);
    
    // Asking for a version too old
    assert(SchemaVersioning::check_schema_compatibility(target_past) == SchemaCompatibilityStatus::REJECTED_OUTRIGHT);

    // Asking for current version
    assert(SchemaVersioning::check_schema_compatibility(target_current) == SchemaCompatibilityStatus::COMPATIBLE);
    
    std::cout << "Schema minimum bounds test passed." << std::endl;
}

void test_adapter_compliance() {
    SourceCapabilityProfile compliant_profile{
        .adapter_name = "GoodAdapter",
        .schema_compliance = {"v1.0.0", "", false}, 
        .time_proofs = {true, true, true},
        .linkage_proofs = {true, true, true, true}
    };

    assert(SchemaVersioning::evaluate_adapter_compliance(compliant_profile) == SchemaCompatibilityStatus::COMPATIBLE);

    SourceCapabilityProfile provisional_profile{
        .adapter_name = "CME_iLink3",
        .schema_compliance = {"v1.0.0", "Provisional precision mapping active.", true}, 
        .time_proofs = {true, true, true},
        .linkage_proofs = {true, true, true, true}
    };
    
    // Test that the deprecation pathway is correctly identified
    assert(SchemaVersioning::evaluate_adapter_compliance(provisional_profile) == SchemaCompatibilityStatus::DEPRECATED_BUT_ACCEPTED);

    SourceCapabilityProfile old_profile{
        .adapter_name = "OldAdapter",
        .schema_compliance = {"v0.9.0", "", false}, 
        .time_proofs = {true, true, true},
        .linkage_proofs = {true, true, true, true}
    };
    
    assert(SchemaVersioning::evaluate_adapter_compliance(old_profile) == SchemaCompatibilityStatus::REJECTED_OUTRIGHT);

    std::cout << "Adapter compliance mapping test passed." << std::endl;
}

int main() {
    test_schema_minimums();
    test_adapter_compliance();
    
    // Verify deprecation warning string exists
    assert(SchemaVersioning::PROVISIONAL_NUMERICS_DEPRECATION_WARNING.find("WARNING") != std::string_view::npos);

    std::cout << "All Schema Versioning tests passed." << std::endl;
    return 0;
}
