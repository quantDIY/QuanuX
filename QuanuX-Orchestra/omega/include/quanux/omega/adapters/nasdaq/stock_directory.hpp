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
    Stale = 0,
    Loading = 1,
    Ready = 2,
    Invalid = 3
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
        _state.store(RegistryReadiness::Loading, std::memory_order_release);
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
        _state.store(RegistryReadiness::Stale, std::memory_order_release);
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
};

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
