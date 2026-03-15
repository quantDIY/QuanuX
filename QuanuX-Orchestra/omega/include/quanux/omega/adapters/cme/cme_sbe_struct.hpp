#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

// A mock struct mimicking CME MDP3 SBE Execution Report layout for the bridge
#pragma pack(push, 1)
struct CmeExecutionReportSbe {
    uint64_t sequence_number;
    uint64_t order_id;
    uint64_t exec_id;
    char cl_ord_id[20];
    uint64_t transact_time;
    uint8_t template_id;
    uint8_t side; // 1=Buy, 2=Sell
    uint8_t ord_status;  // 0=New, 1=PartiallyFilled, 2=Filled, 4=Canceled, 8=Rejected
    uint16_t md_error_code; 
    uint16_t unmapped_native_code; // SBE field that has no Omega translation
    
    // Numeric Placeholder types matching what Omega currently utilizes.
    double order_qty_placeholder;
    double price_placeholder;
};
#pragma pack(pop)

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
