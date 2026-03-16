#pragma once

#include <chrono>
#include <string_view>
#include "quanux/omega/omega_core/omega_event_time.hpp"
#include "quanux/omega/adapters/ibkr/ibkr_structs.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace ibkr {

class IbkrTimestampMapper {
public:
    static void map_time(const IbkrExecutionReportMock* msg, core::OmegaEventTime& time) noexcept {
        // Map Source Time
        // IBKR provides string dates e.g. "20231025  10:30:00" natively
        // Here we assign the epoch nanos loosely to represent string parsing in a real prod system
        time.source_time.epoch_nanos = 1700000000000000000ULL; // Static mock
        time.source_time.precedence = time::TimestampPrecedence::SourceNative;
        
        // Calculate Adapter Time (Unix Epoch nanoseconds of right now, simulating parsing completion)
        auto now = std::chrono::system_clock::now();
        time.adapter_time.epoch_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        time.adapter_time.precedence = time::TimestampPrecedence::AdapterProcess;
    }
};

} // namespace ibkr
} // namespace adapters
} // namespace omega
} // namespace quanux
