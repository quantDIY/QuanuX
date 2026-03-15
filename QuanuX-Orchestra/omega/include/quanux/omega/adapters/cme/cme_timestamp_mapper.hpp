#pragma once

#include <chrono>
#include "quanux/omega/omega_core/omega_event_time.hpp"
#include "quanux/omega/adapters/cme/cme_sbe_struct.hpp"

namespace quanux {
namespace omega {
namespace adapters {
namespace cme {

class CmeTimestampMapper {
public:
    static void map_time(const CmeExecutionReportSbe* msg, core::OmegaEventTime& time) noexcept {
        // Map Source Time
        time.source_time.epoch_nanos = msg->transact_time;
        time.source_time.precedence = time::TimestampPrecedence::SourceNative;
        
        // Calculate Adapter Time (Unix Epoch nanoseconds of right now, simulating parsing completion)
        auto now = std::chrono::system_clock::now();
        time.adapter_time.epoch_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        time.adapter_time.precedence = time::TimestampPrecedence::AdapterProcess;
    }
};

} // namespace cme
} // namespace adapters
} // namespace omega
} // namespace quanux
