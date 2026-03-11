#pragma once
#include <string_view>
#include <array>

namespace quanux {
namespace orchestra {
namespace ibkr_onixs_figi {

struct TickerMap {
    std::string_view venue_ticker;
    std::string_view figi;
};

inline constexpr std::array<TickerMap, 3> venue_to_figi = {{
    {"ES M4", "BBG001"},
    {"ESM4", "BBG001"},
    {"NQZ4", "BBG002"}
}};

inline constexpr std::string_view resolve_figi(std::string_view ticker) {
    size_t left = 0;
    size_t right = venue_to_figi.size();
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (venue_to_figi[mid].venue_ticker < ticker) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    if (left < venue_to_figi.size() && venue_to_figi[left].venue_ticker == ticker) {
        return venue_to_figi[left].figi;
    }
    return "UNKNOWN_FIGI";
}

} // namespace ibkr_onixs_figi
} // namespace orchestra
} // namespace quanux
