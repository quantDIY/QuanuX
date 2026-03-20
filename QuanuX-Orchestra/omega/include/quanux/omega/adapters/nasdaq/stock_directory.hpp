#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <mutex>
#include <atomic>

namespace quanux {
namespace omega {
namespace adapters {
namespace nasdaq {

enum class RegistryReadiness {
    ColdStart = 0,
    PartialPreload = 1,
    Ready = 2,
    Degraded = 3,
    RecoverySync = 4
};

enum class DegradationReason {
    None = 0,
    SequenceGap = 1,
    HeartbeatTimeout = 2,
    MulticastDrop = 3,
    OperatorOverride = 4
};

class StockDirectoryRegistry {
public:
    static StockDirectoryRegistry& getInstance() {
        static StockDirectoryRegistry instance;
        return instance;
    }

    void clear_for_new_trading_day() {
        std::lock_guard<std::mutex> lock(_mutex);
        for(size_t i=0; i<65536; ++i) {
            _directory[i].symbol = "";
            _directory[i].last_update_nanos = 0;
        }
        _state.store(RegistryReadiness::ColdStart, std::memory_order_release);
    }
    
    void begin_partial_preload() {
        _state.store(RegistryReadiness::PartialPreload, std::memory_order_release);
    }

    // DOCTRINE: Timestamp-Aware Overwrite. Rejects older UDP replays dynamically natively.
    bool declare_locate(uint16_t stock_locate, const std::string& symbol, uint64_t timestamp_nanos) {
        if (stock_locate == 0) return false;
        std::lock_guard<std::mutex> lock(_mutex);
        
        auto& entry = _directory[stock_locate];
        
        if (!entry.symbol.empty()) {
            if (timestamp_nanos <= entry.last_update_nanos) {
                return false; // Rejects strictly stale UDP injections correctly natively.
            }
        }
        
        entry.symbol = symbol;
        entry.last_update_nanos = timestamp_nanos;
        return true;
    }

    // Operator Lifecycle Gates
    void mark_ready() {
        _state.store(RegistryReadiness::Ready, std::memory_order_release);
        _last_reason.store(DegradationReason::None, std::memory_order_release);
    }

    void trigger_degradation(DegradationReason reason) {
        _last_reason.store(reason, std::memory_order_release);
        _state.store(RegistryReadiness::Degraded, std::memory_order_release);
    }

    void begin_recovery_sync(uint64_t target_sequence) {
        _target_sync_sequence.store(target_sequence, std::memory_order_release);
        _state.store(RegistryReadiness::RecoverySync, std::memory_order_release);
    }

    bool check_catchup_completion(uint64_t current_sequence) {
        _current_sync_sequence.store(current_sequence, std::memory_order_release);
        if (_state.load(std::memory_order_acquire) == RegistryReadiness::RecoverySync) {
            if (current_sequence >= _target_sync_sequence.load(std::memory_order_acquire)) {
                mark_ready(); // Automatically transitions
                return true;
            }
        }
        return false;
    }

    DegradationReason get_last_reason() const {
        return _last_reason.load(std::memory_order_acquire);
    }

    RegistryReadiness get_readiness_state() const {
        return _state.load(std::memory_order_acquire);
    }

    bool is_ready() const {
        return get_readiness_state() == RegistryReadiness::Ready;
    }

    bool try_get_symbol(uint16_t stock_locate, std::string& out_symbol) const {
        if (stock_locate == 0) return false;
        
        std::lock_guard<std::mutex> lock(_mutex);
        const std::string& symbol = _directory[stock_locate].symbol;
        if (symbol.empty()) return false; // Graceful rejection of unmapped limits.
        
        out_symbol = symbol;
        return true;
    }

private:
    StockDirectoryRegistry() {
        _state.store(RegistryReadiness::ColdStart, std::memory_order_release);
        clear_for_new_trading_day();
    }
    StockDirectoryRegistry(const StockDirectoryRegistry&) = delete;
    StockDirectoryRegistry& operator=(const StockDirectoryRegistry&) = delete;

    struct LocateEntry {
        std::string symbol;
        uint64_t last_update_nanos;
    };

    std::array<LocateEntry, 65536> _directory;
    mutable std::mutex _mutex;
    std::atomic<RegistryReadiness> _state;
    std::atomic<DegradationReason> _last_reason{DegradationReason::None};
    std::atomic<uint64_t> _target_sync_sequence{0};
    std::atomic<uint64_t> _current_sync_sequence{0};
};

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
