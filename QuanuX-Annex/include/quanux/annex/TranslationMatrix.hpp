#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>

namespace quanux {
namespace annex {

// Omega Translation Matrix
// Maps static venue codes (e.g., NASDAQ, BATS) to ISO-20022 identifiers natively in O(1) time.
class TranslationMatrix {
public:
    static TranslationMatrix& getInstance();

    // Fetch the Omega Pre-Market maps and lock them
    void initialize();

    // The Fail-Open Protocol mapping
    // Returns 0 (undefined) if unknown so the loop doesn't block
    int32_t mapVenueToInstrumentId(const std::string& venueCode) const;

    // Phase 5 Route & Venue Validation Constraints
    // Explicit Numeric Registry Declarations (Non-Eternal)
    static constexpr uint32_t VENUE_NASDAQ_XNAS = 1001;
    static constexpr uint32_t VENUE_IEX_IEXG = 1005;

    bool isValidVenue(uint32_t venue_id) const;
    bool isValidRoute(uint32_t route_id) const;
    bool isValidCounterparty(uint32_t counterparty_id) const;
    bool isValidIdentitySet(uint32_t venue_id, uint32_t route_id, uint32_t counterparty_id) const;

private:
    TranslationMatrix() = default;
    ~TranslationMatrix() = default;

    std::unordered_map<std::string, int32_t> venueMap_;
    bool isInitialized_ = false;
};

} // namespace annex
} // namespace quanux
