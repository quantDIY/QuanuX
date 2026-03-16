#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace quanux {
namespace omega {
namespace adapters {
namespace nyse {

// Broadened mock struct representing downstream NYSE pillar execution data
// Focuses on mapping venue realities into Omega.
#pragma pack(push, 1)
struct NysePillarMock {
    uint32_t msg_type; // 0x200 = New Order, 0x201 = Replace, 0x202 = Cancel, 0x210 = Execution
    uint64_t source_time_ns;
    
    // Broadened identity mapping
    char symbol[11];
    uint64_t me_order_id; // Matching Engine Order ID
    uint64_t client_order_id; 
    
    // Semantics & Limits
    uint32_t side; // 1 = Buy, 2 = Sell, 3 = Sell Short
    uint32_t qty;
    uint64_t price; // Implicit 8-decimal scaling natively on NYSE Pillar
    
    // Order State
    uint8_t execution_status; // '0' = New, '1' = Partial, '2' = Fill, '4' = Cancel, '8' = Reject
    char reason_code[4]; // Alphanumeric reject reason mappings
};
#pragma pack(pop)

} // namespace nyse
} // namespace adapters
} // namespace omega
} // namespace quanux
