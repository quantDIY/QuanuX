#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace lse {

#pragma pack(push, 1)
// Mock LSE Trading Service execution structure
// Intentionally uses non-U.S. standard string mappings for execution status
// to prove the generic normalizer bounds hold on international equities
struct LseExecutionMock {
    uint8_t message_type; // 15 = Execution Report
    uint64_t transact_time_nanos;

    char order_id[20];
    char instrument_id[12]; // e.g., 'VOD.L'
    
    char side; // '1' = Buy, '2' = Sell 
    double exec_price; // Native double, requires capability warning
    int32_t exec_qty;
    
    char exec_type[2]; // "0 " = New, "1 " = Partial, "2 " = Fill, "8 " = Reject
    char reject_code[4]; 
};
#pragma pack(pop)

} // namespace lse
} // namespace adapters
} // namespace omega
} // namespace quanux
