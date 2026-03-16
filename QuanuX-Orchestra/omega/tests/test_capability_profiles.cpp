#include <iostream>
#include <cassert>
#include "quanux/omega/adapters/cme/cme_adapter.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_adapter.hpp"

using namespace quanux::omega;

// This assertion suite validates that capabilities describe PROOF, not theory.
void test_cme_capabilities() {
    auto profile = adapters::cme::CmeAdapter::get_capability_profile();
    
    // CME iLink can prove deterministic replay state because of sequence number
    // tracking on binary streams.
    assert(profile.linkage_proofs.maintains_deterministic_replay_state == true);
    
    // CME natively includes transact timing hardware proofs.
    assert(profile.time_proofs.proves_source_native == true);

    std::cout << "CME Capability Profile Assertions passed." << std::endl;
}

void test_ibkr_capabilities() {
    auto profile = adapters::ibkr::IbkrAdapter::get_capability_profile();
    
    // IBKR over TCP text sockets lacks deterministic sequence replay state guarantees
    assert(profile.linkage_proofs.maintains_deterministic_replay_state == false);
    
    // IBKR API cannot natively prove true exchange source logic gate routing time
    assert(profile.time_proofs.proves_source_native == false);

    std::cout << "IBKR Capability Profile Assertions passed." << std::endl;
}

int main() {
    test_cme_capabilities();
    test_ibkr_capabilities();
    
    std::cout << "Capability Formalization validation complete." << std::endl;
    return 0;
}
