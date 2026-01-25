---
name: quanux-indicators
description: "Modern C++20 Indicators Library (Lazy Evaluation, Policy-Based)"
---

# Quanux Indicators (`libquanux-indicators`)

## Overview
A high-performance, C++20 header-only library replacing TA-Lib. It supports **Lazy Evaluation** (via ranges) and **Policy-Based Design** (for Market Profile).

## 1. Standard Indicators (SMA, RSI, etc.)

### A. Lazy Evaluation (The Pipe Syntax)
**Use when**: Building complex pipelines or needing zero-allocation loop fusion.
```cpp
#include "quanux/indicators/view.hpp"
#include "quanux/indicators/sma.hpp"

// Setup view (O(1), no allocation)
auto view = prices | quanux::indicators::sma(10);

// Iterate (Computation happens here)
for (auto val : view) { ... }
```

### B. Eager Evaluation (Traditional)
**Use when**: You just need a `std::vector` result immediately.
```cpp
#include "quanux/indicators/sma.hpp"

std::vector<double> results = quanux::indicators::compute_sma(prices, 10);
```

**WARNING**: Eager evaluation allocates memory for the entire result vector.

## 2. Market & Volume Profile (Auction Market Theory)

This library uses **Policy-Based Design** to let you choose performance characteristics.

### Storage Policies
1.  **DenseStorage** (`std::vector`): Fast, contiguous memory. Use for tight price ranges (Futures, Equities).
2.  **SparseStorage** (`std::map`): Infinite range, slower access. Use for Crypto or sparsely traded assets.

### Usage
```cpp
#include "quanux/indicators/market_profile.hpp"
#include "quanux/indicators/volume_profile.hpp"

// TPO (Time Price Opportunity)
using namespace quanux::indicators;
MarketProfile<DenseStorage> es_profile(0.25); // Tick size 0.25
es_profile.process(price);

// Volume Profile
VolumeProfile<SparseStorage> btc_profile(0.01); 
btc_profile.process(price, volume);
```

## 3. Extensibility (The Registry)

To add new indicators, do NOT modify the core.
1.  Drop header files into `server/indicators/include/contrib/<name>/`.
2.  The build system automatically picks them up.
3.  Include them via `#include "contrib/<name>/my_indicator.hpp"`.

## 4. Best Practices for Agents
*   **Always prefer Lazy Evaluation** for strategy chains to minimize memory bandwidth.
*   **Use Eager Wrappers** only when interfacing with legacy code or Python bindings that expect vectors.
*   **Check inputs**: The library assumes valid numeric inputs (`std::vector<double>`, etc.).
*   **NaN Handling**: The first `period - 1` values of extensive indicators (SMA, etc.) are `NaN`. Always check `std::isnan()` if iterating manually.

## 5. Integration with QuanuX Organism

The Indicator Engine is designed as a **Core Subsystem** but is physically decoupled.

*   **Strategy Builder**: Consumes indicators via Python Bindings (generated from these C++ headers).
*   **Backtester**: Links directly against `libquanux-indicators` for zero-latency execution.
*   **Execution Nodes**: Compile strategies that inline these indicator views.

## 6. Lifecycle Management (quanuxctl)

This module strictly adheres to the QuanuX Modular Protocol. It can be removed or replaced at any time.

*   **Install Suite**: `quanuxctl module install indicators`
    *   Pulls source, configures CMake, builds tests.
*   **Uninstall Suite**: `quanuxctl module remove indicators`
    *   Wipes `server/indicators` directory clean.
    *   Updates `CMakeLists.txt` to remove the subdirectory.
*   **Verify Integrity**: `quanuxctl module check indicators`
    *   Runs the `test_registry` and unit tests.
    *   Runs the `test_registry` and unit tests.

## 7. Python Bindings (Strategy Builder)

When generating Python strategy code (e.g. for `server/strategies/`), you can use the optimized C++ bindings.

```python
import quanux_indicators as qi

# 1. Eager Computing
sma_values = qi.compute_sma(prices_list, 10)

# 2. Market Profile
mp = qi.DenseMarketProfile(100.0, 0.25)
for price in ticks:
    mp.process(price)
count = mp.query(100.0)
```
