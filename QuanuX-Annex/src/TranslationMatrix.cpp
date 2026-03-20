#include "quanux/annex/TranslationMatrix.hpp"
#include <iostream>

namespace quanux {
namespace annex {

TranslationMatrix& TranslationMatrix::getInstance() {
    static TranslationMatrix instance;
    return instance;
}

void TranslationMatrix::initialize() {
    if (isInitialized_) return;
    
    // In production, this issues an HTTP/GRPC call to the Omega service.
    // For deterministic locking, we statically define the map pre-market here.
    std::cout << "[OMEGA] Fetching Pre-Market Translation Map..." << std::endl;
    venueMap_["NASDAQ"] = 1001;
    venueMap_["BATS"] = 1002;
    venueMap_["ARCA"] = 1003;
    venueMap_["EDGX"] = 1004;
    venueMap_["IEX"] = 1005;

    isInitialized_ = true;
    std::cout << "[OMEGA] Translation Matrix Locked into C++ Memory." << std::endl;
}

int32_t TranslationMatrix::mapVenueToInstrumentId(const std::string& venueCode) const {
    auto it = venueMap_.find(venueCode);
    if (it != venueMap_.end()) {
        return it->second;
    }
    // Fail-Open Protocol: Unknown code. Flag in metadata but return 0.
    // The Event Loop must never halt.
    return 0; // 0 designates undefined instrument_id
}

bool TranslationMatrix::isValidVenue(uint32_t venue_id) const {
    // Current Declared Registry Limits
    return venue_id == VENUE_NASDAQ_XNAS || venue_id == VENUE_IEX_IEXG;
}

bool TranslationMatrix::isValidRoute(uint32_t route_id) const {
    // Native route constraint: valid paths are bounded strictly below 1000
    return route_id > 0 && route_id < 1000;
}

bool TranslationMatrix::isValidCounterparty(uint32_t counterparty_id) const {
    // Goldman/JPM execution connectivity bounds
    return counterparty_id > 0;
}

bool TranslationMatrix::isValidIdentitySet(uint32_t venue_id, uint32_t route_id, uint32_t counterparty_id) const {
    if (!isValidRoute(route_id)) return false;

    if (venue_id > 0) {
        // Native Exchange or Dark Pool Routing Path
        if (!isValidVenue(venue_id)) return false;
        
        // Option A Enforcement: counterparty_id must be forbidden (0) for venue-direct
        if (counterparty_id != 0) return false;
        
        // Synthetic Route Override Avoidance
        if (venue_id >= 1001 && venue_id <= 1005 && route_id < 100) return false;
    } else {
        // Broker/Dealer Path (Venue = 0)
        // Counterparty is strictly required
        if (!isValidCounterparty(counterparty_id)) return false;
    }

    return true;
}

} // namespace annex
} // namespace quanux
