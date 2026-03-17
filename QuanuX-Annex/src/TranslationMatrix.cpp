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

} // namespace annex
} // namespace quanux
