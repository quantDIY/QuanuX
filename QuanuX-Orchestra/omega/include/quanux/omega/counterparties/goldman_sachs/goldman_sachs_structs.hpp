#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace counterparties {
namespace goldman_sachs {

#pragma pack(push, 1)
// Mock shape for a Goldman Sachs counterparty connectivity path ingress.
// Models a routing broker path where venue may be unknown but counterparty
// and route identities are explicit.
struct GoldmanSachsIngressMock {
    uint8_t msg_type; // e.g., 8 for Execution
    uint64_t route_transit_time_nanos;

    char client_order_id[24];
    char gs_execution_id[24];
    
    char routing_strategy[16];   // e.g., 'SIGMA_X', 'SONIC'
    char execution_venue_mic[4]; // Optional venue, e.g., 'XOFF'

    char side; 
    double fill_price;
    int32_t fill_qty;
    
    char exec_type; // 'NEW', 'FILL', 'PARTIAL', 'CANCELED', 'REJECTED'
    char reject_reason_code[4]; 
};
#pragma pack(pop)

} // namespace goldman_sachs
} // namespace counterparties
} // namespace omega
} // namespace quanux
