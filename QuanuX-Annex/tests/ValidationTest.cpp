#include <gtest/gtest.h>
#include "quanux/annex/TranslationMatrix.hpp"
#include "quanux/annex/TelemetryExhaust.hpp"

using namespace quanux::annex;

TEST(ConsumerIdentityTest, ValidatesExpandedVenueIdentities) {
    auto& tMatrix = TranslationMatrix::getInstance();
    
    // Explicit Validation of Phase 5 Venue Paths
    EXPECT_TRUE(tMatrix.isValidVenue(TranslationMatrix::VENUE_NASDAQ_XNAS)); // NASDAQ
    EXPECT_TRUE(tMatrix.isValidVenue(TranslationMatrix::VENUE_IEX_IEXG)); // IEX
    EXPECT_FALSE(tMatrix.isValidVenue(1002)); // BATS explicit drop (not yet staged)
    EXPECT_FALSE(tMatrix.isValidVenue(0)); // Missing venue_id rejected

    // Route Extensibility Checks
    EXPECT_TRUE(tMatrix.isValidRoute(500));
    EXPECT_FALSE(tMatrix.isValidRoute(0)); // Zero bounds forbidden - missing route_id

    // Counterparty Validations (Goldman/JPM Execution isolation)
    EXPECT_TRUE(tMatrix.isValidCounterparty(10));
    EXPECT_FALSE(tMatrix.isValidCounterparty(0)); // Missing counterparty_id rejected
}

TEST(ConsumerIdentityTest, NasdaqStagingAcceptance) {
    auto& tMatrix = TranslationMatrix::getInstance();
    
    // VALID NASDAQ PAYLOAD: xNAS venue + Valid physical network route (>100)
    EXPECT_TRUE(tMatrix.isValidIdentitySet(TranslationMatrix::VENUE_NASDAQ_XNAS, 110, 0));
    
    // REJECTED NASDAQ PAYLOAD: xNAS venue + synthetic internal route (<10)
    EXPECT_FALSE(tMatrix.isValidIdentitySet(TranslationMatrix::VENUE_NASDAQ_XNAS, 5, 0));
}

TEST(ConsumerIdentityTest, RejectsContradictoryIdentityCombinations) {
    auto& tMatrix = TranslationMatrix::getInstance();
    // Valid: NASDAQ (1001), Route (201), Counterparty (301)
    EXPECT_TRUE(tMatrix.isValidIdentitySet(1001, 201, 301));
    // Invalid: Native exchange (1002) with synthetic dark-pool route (<100)
    EXPECT_FALSE(tMatrix.isValidIdentitySet(1002, 5, 301)); 
    // Invalid: Valid route and counterparty but 0 venue
    EXPECT_FALSE(tMatrix.isValidIdentitySet(0, 201, 301));
}

TEST(ConsumerIdentityTest, StructuralMemoryLayoutUnchanged) {
    // Asserting the byte layout matches Python Mock Injector `<QIIIIddIIB` exactly.
    // 8 + 4 + 4 + 4 + 4 + 8 + 8 + 4 + 4 + 1 = 49 bytes.
    // Due to #pragma pack(push, 1), sizeof should be exactly 49.
    EXPECT_EQ(sizeof(MarketTick), 49);
}
