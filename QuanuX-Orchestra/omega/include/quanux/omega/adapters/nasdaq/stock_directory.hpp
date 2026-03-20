#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <mutex>

namespace quanux {
namespace omega {
namespace adapters {
namespace nasdaq {

class StockDirectoryRegistry {
public:
    static StockDirectoryRegistry& getInstance() {
        static StockDirectoryRegistry instance;
        return instance;
    }

    // Explicit Daily Preload Lifecycle
    void clear_for_new_trading_day() {
        std::lock_guard<std::mutex> lock(_mutex);
        _directory.fill(""); // O(1) allocation bounds, clears all 65536 entries
    }

    // Handles ITCH Array Message (Type 'R')
    void declare_locate(uint16_t stock_locate, const std::string& symbol) {
        if (stock_locate == 0) return; // 0 is invariably discarded by ITCH docs
        std::lock_guard<std::mutex> lock(_mutex);
        _directory[stock_locate] = symbol; // Overwrites any stale/duplicate definitions inherently resolving staleness.
    }

    // O(1) Execution Hot Path Array Access (Thread-Safe read logic generally lock-free in production if guaranteed preloaded)
    bool try_get_symbol(uint16_t stock_locate, std::string& out_symbol) const {
        if (stock_locate == 0) return false;
        
        // Normally lock-free if single-writer pre-market, placing lock strictly for staging mock proof
        std::lock_guard<std::mutex> lock(_mutex);
        const std::string& symbol = _directory[stock_locate];
        if (symbol.empty()) return false;
        
        out_symbol = symbol;
        return true;
    }

private:
    StockDirectoryRegistry() {
        _directory.fill("");
    }
    
    // Disable copy/move
    StockDirectoryRegistry(const StockDirectoryRegistry&) = delete;
    StockDirectoryRegistry& operator=(const StockDirectoryRegistry&) = delete;

    // uint16_t maps strictly from 0 to 65535 natively fitting entirely natively bypassing hashing limits natively.
    std::array<std::string, 65536> _directory;
    mutable std::mutex _mutex; // Mutable allowing const reads safely simulating locks globally
};

} // namespace nasdaq
} // namespace adapters
} // namespace omega
} // namespace quanux
