#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace nymex {

#pragma pack(push, 1)
// Mock NYMEX clearing execution shape
// Uses short string tags for identity and separate floating point for demonstration
// of capability variance.
struct NymexExecutionMock {
    uint8_t transaction_type; // 5 = Clearing Exec
    uint64_t transact_time_nanos;

    char trade_id[16];
    char contract_symbol[6]; // 'CL' (Crude), 'NG' (Natural Gas)
    
    char buy_sell; // 'B' or 'S'
    double execution_price; // Native double, requires capability warning
    int32_t quantity;
    
    uint8_t execution_status; // 0 = Accepted, 1 = Cancelled, 2 = Rejected
    uint16_t reject_code;
};
#pragma pack(pop)

} // namespace nymex
} // namespace adapters
} // namespace omega
} // namespace quanux
