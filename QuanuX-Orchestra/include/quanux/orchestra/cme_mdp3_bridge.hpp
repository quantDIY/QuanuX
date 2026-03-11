/**
 * QuanuX-Orchestra: Engine Bridge
 * Venue: cme_mdp3
 */

#pragma once

#include "quanux/orchestra/constants.hpp"

namespace quanux {
namespace orchestra {
namespace cme_mdp3_bridge {

inline constexpr quanux::orchestra::Side translate_side(char venue_side) {
    switch (venue_side) {
        case '1': return quanux::orchestra::Side::Buy;
        case '2': return quanux::orchestra::Side::Sell;
        default: return static_cast<quanux::orchestra::Side>('?');
    }
}

} // namespace cme_mdp3_bridge
} // namespace orchestra
} // namespace quanux
