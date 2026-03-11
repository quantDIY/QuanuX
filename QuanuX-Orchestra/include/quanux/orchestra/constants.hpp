/**
 * QuanuX-Orchestra: The Rosetta Stone
 * Auto-generated FIX Orchestra Constants
 * SHA-256 Checksum: a52a96d080c410e0f95298009ff33454a6ad52c2572c29638f7cfdb4aae69dab
 * Generated: 2026-03-11T16:10:18.433878Z
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
    QuanuxUnmappedTag = 99999,
};

enum class Side : char {
    Buy = '1',
    Sell = '2',
};

} // namespace orchestra
} // namespace quanux
