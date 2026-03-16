#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace lme {

#pragma pack(push, 1)
// Mock LME LMEselect API execution report boundary
struct LmeExecutionMock {
    uint16_t msg_type; // 8 = Execution Report
    uint64_t transact_time_nanos;

    uint64_t order_id;
    char instrument_code[6]; // e.g., 'CA' (Copper), 'AH' (Primary Aluminium)
    
    char side; // '1' = Buy, '2' = Sell 
    double price; // Native double, requires capability warning
    int32_t quantity;
    
    char exec_type; // '0' = New, '1' = Partial, '2' = Fill, '8' = Reject
    uint16_t reject_reason;
};
#pragma pack(pop)

} // namespace lme
} // namespace adapters
} // namespace omega
} // namespace quanux
