#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace cbot {

// A mock ingress structure representing CBOT SBE-like execution data.
// Maintains distinct venue presence despite potential parent-group similarities.
#pragma pack(push, 1)
struct CbotExecutionMock {
    uint16_t msg_type; // 1 = Execution Report
    uint64_t transact_time_nanos;

    // Identity Boundaries
    uint64_t cbot_order_id; // Venue-specific sequence
    char product_group[4];  // E.g., 'ZC' (Corn), 'ZS' (Soybeans)

    // Semantics
    uint8_t side; // 1 = Buy, 2 = Sell
    int32_t fill_qty;
    int64_t fill_price; // Assumed 8 decimal scaling
    
    // Status
    uint8_t status_code; // '0' = New, '4' = Filled, '8' = Rejected
    uint16_t error_code; // If rejected
};
#pragma pack(pop)

} // namespace cbot
} // namespace adapters
} // namespace omega
} // namespace quanux
