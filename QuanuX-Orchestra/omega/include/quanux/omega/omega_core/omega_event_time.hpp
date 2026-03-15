#pragma once

#include "quanux/omega/omega_time/timestamp_precedence.hpp"

namespace quanux {
namespace omega {
namespace core {

struct OmegaEventTime {
    time::TimestampNode source_time;
    time::TimestampNode venue_time;
    
    // Hardware-quality receive timestamp (e.g. NIC MAC receipt time)
    time::TimestampNode receive_time;
    
    // The timestamp when the adapter completed source-specific decoding 
    // prior to canonical envelope assembly.
    time::TimestampNode adapter_time;
    
    time::TimestampNode normalization_time;

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return normalization_time.is_valid();
    }
};

} // namespace core
} // namespace omega
} // namespace quanux
