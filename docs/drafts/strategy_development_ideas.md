# Skill: C++ Strategy Development

**Goal**: Create a high-performance trading strategy compatible with the QuanuX Universal Runner.

## 1. The Strategy Template

Use the following boilerplate for any new strategy (`execution-node/cpp/strategies/my_strategy.cpp`):

```cpp
#include "strategy_interface.h"
#include <iostream>
#include <memory>
// Include your indicators
#include "quanux/indicators/market_profile.hpp"

// 1. Definition
class MyStrategy {
    // State goes here
    double vwap_ = 0.0;
public:
    void init(const OrderService* service) {
        std::cout << "[MyStrat] Ready.\n";
    }

    void on_market_data(const MarketUpdate* update) {
        if (!update->is_trade) return;
        
        // Logic
        if (update->price > vwap_) {
             // ...
        }
    }
};

// 2. ABI Wiring (Boilerplate)
extern "C" {
    StrategyContext* my_create() { return (StrategyContext*)new MyStrategy(); }
    void my_destroy(StrategyContext* ctx) { delete (MyStrategy*)ctx; }
    void my_init(StrategyContext* ctx, const OrderService* s) { ((MyStrategy*)ctx)->init(s); }
    void my_md(StrategyContext* ctx, const MarketUpdate* u) { ((MyStrategy*)ctx)->on_market_data(u); }
    // ... Map other signals

    Strategy* create_strategy() {
        static Strategy s = {
            .name = "MyStrategy",
            .create_context = my_create,
            .destroy_context = my_destroy,
            .on_init = my_init,
            .on_market_data = my_md,
        };
        return &s;
    }
}
```

## 2. CMake Integration

Add your strategy to `execution-node/cpp/CMakeLists.txt`:

```cmake
add_library(my_strategy SHARED strategies/my_strategy.cpp)
target_include_directories(my_strategy PRIVATE include)
target_link_libraries(my_strategy PRIVATE quanux_indicators_lib) // Optional: Link indicators
set_target_properties(my_strategy PROPERTIES PREFIX "") // Ensures my_strategy.so / .dylib
```

## 3. Best Practices (HFT)

*   **Avoid Heap Allocation**: `new` / `malloc` in `on_market_data` is forbidden. Pre-allocate in `init`.
*   **Fast Path**: `if (!is_trade) return;` should be the first line if you only trade ticks.
*   **Logging**: `std::cout` is slow. In production, use the `Logger` service (coming in Phase 4).
*   **Indicators**: Use `quanux_indicators` library which is designed for incremental updates (Zero-History needed).
    *   Example: `MarketProfile<DenseStorage>` is O(1) for updates.

## 4. Testing

Always verify your strategy offline first!

1.  **Download Sample Data**: Fetch a small `.dbn` file (MBO or Trades) from Databento.
2.  **Run Offline**:
    ```bash
    ./quanux_runner --strategy my_strategy.dylib --file sample.dbn
    ```
3.  **Validate**: Check logs for "Initialized Strategy" and ensuring logic triggers as expected.
