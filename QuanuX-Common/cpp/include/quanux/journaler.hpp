/**
 * @file journaler.hpp
 * @brief The QuanuX Black Box Ledger.
 *
 * "There's something I want, and you're gonna have to let me do it."
 *
 * The Journaler owns the truth of the session. It records an append-only binary
 * log (.qlog) capturing every state transition and outbound OrderPacket with
 * zero allocation jitter.
 */

#pragma once

#include "quanux/sovereign_state.hpp"
#include <cstdint>
#include <cstring>
#include <fstream>

namespace quanux {
namespace audit {

#pragma pack(push, 1) // Force 1-byte alignment for the binary ledger
struct LedgerEntry {
  uint64_t tsc_timestamp;       // Hardware cycle timing
  quanux::ExecutionState state; // The current FSM Ritchie State
  int32_t current_position;     // Absolute risk position
  uint8_t interlock_status;     // Sentinel Gate Status
  char order_packet[32];        // FIX payload snippet / Outbound intention
};
#pragma pack(pop)

class BinaryJournaler {
public:
  BinaryJournaler(const char *filepath) {
    // Open in append-only binary mode
    log_file_.open(filepath, std::ios::out | std::ios::binary | std::ios::app);
  }

  ~BinaryJournaler() {
    if (log_file_.is_open()) {
      log_file_.flush();
      log_file_.close();
    }
  }

  // Must be invoked inline without heap allocation
  inline void record_transition(const quanux::SovereignState &state,
                                const char *packet = nullptr) {
    LedgerEntry entry{};
    entry.tsc_timestamp = __builtin_ia32_rdtsc();
    entry.state = state.execution_state.load(std::memory_order_relaxed);
    entry.current_position =
        state.current_position.load(std::memory_order_relaxed);
    entry.interlock_status =
        state.risk_interlock.load(std::memory_order_relaxed);

    if (packet) {
      std::strncpy(entry.order_packet, packet, sizeof(entry.order_packet) - 1);
    }

    log_file_.write(reinterpret_cast<const char *>(&entry),
                    sizeof(LedgerEntry));
    // We do not flush here to avoid I/O blocking in the hot path.
    // Operating system page buffers handle the flush.
  }

private:
  std::ofstream log_file_;
};

} // namespace audit
} // namespace quanux
