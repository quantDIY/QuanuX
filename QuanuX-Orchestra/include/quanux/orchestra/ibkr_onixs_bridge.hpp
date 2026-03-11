/**
 * QuanuX-Orchestra: Engine Bridge
 * Venue: ibkr_onixs
 */

#pragma once

#include "quanux/orchestra/constants.hpp"

namespace quanux {
namespace orchestra {
namespace ibkr_onixs_bridge {

inline constexpr quanux::orchestra::Side translate_side(char venue_side) {
    switch (venue_side) {
        case '1': return quanux::orchestra::Side::Buy;
        case '2': return quanux::orchestra::Side::Sell;
        default: return static_cast<quanux::orchestra::Side>('?');
    }
}

} // namespace ibkr_onixs_bridge
} // namespace orchestra
} // namespace quanux
