#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace counterparties {
namespace jpmorgan {

#pragma pack(push, 1)
// Mock shape for a J.P. Morgan counterparty connectivity path ingress.
// Models routing where execution venue may be explicitly attached 
// but JPM remains the executing counterparty identity.
struct JPMorganIngressMock {
    uint8_t payload_code; // e.g., 200 for Execution
    uint64_t router_timestamp_nanos;

    char jpm_order_id[32];
    char execution_venue[8]; // e.g. 'XNYS', 'XNAS'
    
    char order_status; // 'N', 'P', 'F', 'R' (Reject), 'C' (Cancel)
    char side; // 'B' or 'S'

    double execution_price;
    int32_t execution_quantity;
    
    char reject_reason_string[32]; 
};
#pragma pack(pop)

} // namespace jpmorgan
} // namespace counterparties
} // namespace omega
} // namespace quanux
