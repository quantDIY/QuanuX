#pragma once

#include <cstdint>

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

// WARNING:
// This struct defines a strict mock layout for IBKR execution reports to act 
// as our normalized bridge entrance.
// IT IS NOT THE IBKR TRUTH MODEL.
// It exists solely to prove that QuanuX-Omega can accept materially different 
// source shapes into the envelope without redesigning the core contract.

#pragma pack(push, 1)
struct IbkrExecutionReportMock {
    int32_t reqId;
    int32_t permId;
    int32_t clientId;
    int32_t orderId;
    char execId[64];
    char time[32];       // e.g. "20231025  10:30:00"
    char acctNumber[32]; // e.g. "DU12345"
    char exchange[16];   // e.g. "SMART"
    char side[8];        // "BOT" or "SLD"
    
    // Numeric Placeholder types matching what Omega currently utilizes.
    double shares;
    double price;
    double cumQty;
    double avgPrice;

    // Status / Reason String handling
    char status[16];     // "Submitted", "Filled", "Cancelled", "Inactive"
    char whyHeld[64];    // Text reason code or string
    int32_t errorCode;   // Provide code when reasons are mixed
};
#pragma pack(pop)

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
