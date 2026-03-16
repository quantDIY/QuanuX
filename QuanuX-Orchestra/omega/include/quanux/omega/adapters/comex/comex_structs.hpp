#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace comex {

#pragma pack(push, 1)
// Mock COMEX execution structure
// Retains distinct struct isolation to emulate independent gateway boundaries 
struct ComexMatchReport {
    uint16_t msg_type; // 2 = Match Event 
    uint64_t transact_time_nanos;

    uint64_t comex_order_id;
    char product_group[4]; // 'GC' (Gold), 'SI' (Silver)
    
    uint8_t side; // 'B' = Buy, 'S' = Sell
    int32_t qty;
    int64_t price; // Assumed 8 decimal scaling
    
    char order_state; // 'N' = New, 'F' = Filled, 'R' = Rejected
    uint16_t reject_reason;
};
#pragma pack(pop)

} // namespace comex
} // namespace adapters
} // namespace omega
} // namespace quanux
