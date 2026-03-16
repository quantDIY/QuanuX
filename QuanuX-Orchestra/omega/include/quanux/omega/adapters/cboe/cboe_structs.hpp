#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace cboe {

#pragma pack(push, 1)
// Broad ingress mock representing a normalized Cboe venue family execution report.
// Supports both U.S. Equities (BZX, BYX, EDGX, EDGA) and Options (C1, C2, BZX, EDGX)
// semantics without transport session bleeding.
struct CboeExecutionMock {
    uint8_t message_type; // e.g., 0x24 for Execution
    uint64_t transact_time_nanos;

    char order_id[20];
    char symbol[12]; // e.g., 'AAPL' or 'SPXW'
    char asset_class; // 'E' = Equity, 'O' = Options
    char venue_identifier[4]; // 'BZX ', 'EDGX', 'C1  ', 'C2  '
    
    char side; // '1' = Buy, '2' = Sell, '5' = Sell Short
    double exec_price; // Native double, requires capability warning
    int32_t exec_qty;
    
    char exec_report_type; // '0' = New, '1' = Partial, '2' = Fill, '8' = Reject
    char reason_phrase[16]; 
};
#pragma pack(pop)

} // namespace cboe
} // namespace adapters
} // namespace omega
} // namespace quanux
