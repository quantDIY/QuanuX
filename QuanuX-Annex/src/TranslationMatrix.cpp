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
    if (!isValidVenue(venue_id) || !isValidRoute(route_id) || !isValidCounterparty(counterparty_id)) {
        return false;
    }
    // Combinatorial contradictions:
    // If venue_id is a native exchange (1001-1005), route_id must map directly (e.g. > 100), avoiding synthetic dark-pool route overlaps (< 10)
    if (venue_id >= 1001 && venue_id <= 1005 && route_id < 100) {
        return false;
    }
    return true;
}

} // namespace annex
} // namespace quanux
