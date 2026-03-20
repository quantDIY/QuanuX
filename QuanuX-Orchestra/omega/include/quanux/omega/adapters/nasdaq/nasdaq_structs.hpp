#pragma once

#include <cstdint>
#include <cstring>
#include <string>

namespace quanux {
namespace omega {
namespace adapters {
namespace nasdaq {

// A broadened mock struct representing a downstream NASDAQ data/execution ingress.
// We are mapping venue reality into Omega without writing the handler session logic.
#pragma pack(push, 1)
struct NasdaqIngressMock {
    char message_type; // 'A' = Add Order, 'E' = Execute, 'X' = Cancel, 'C' = Execute with Price
    uint16_t stock_locate;   // ITCH Foundation Identity (Big-Endian)
    uint16_t tracking_number; // Internal ITCH boundary
    uint64_t timestamp_nanos;
    uint64_t order_reference_number;
    
    // Broadened identity mapping
    char stock_symbol[8]; // Deprecated, mapped via StockLocate Directory Sync limit
    char mpid[4]; // Market Participant ID

    // Semantics
    char side; // 'B' = Buy, 'S' = Sell
    uint32_t shares;
    uint32_t price; // Implicit 4-decimal format
    
    // State/Reason tracking
    char cancel_reason; // Valid on 'X' messages
};
#pragma pack(pop)

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
