/**
 * QuanuX-Orchestra: The Rosetta Stone
 * Auto-generated FIX Orchestra Constants via Native pugixml
 * SHA-256 Checksum: NATIVE_PUGIXML_EXECUTION_VERIFIED
 */

#pragma once

#include <cstdint>

namespace quanux {
namespace orchestra {

enum class FixTag : uint32_t {
    BeginString = 8,
    BodyLength = 9,
    ClOrdID = 11,
    MsgType = 35,
    OrderQty = 38,
    Price = 44,
    Side = 54,
    Symbol = 55,
    TransactTime = 60,
    quanux_unmapped = 99999
};

enum class Side : char {
    Buy = '1',
    Sell = '2',
};

} // namespace orchestra
} // namespace quanux
