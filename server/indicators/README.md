# Quanux Indicators (`libquanux-indicators`)

A modern C++20 library for High-Frequency Trading (HFT) and Machine Learning features, designed to replace TA-Lib with a focus on **Lazy Evaluation** and **Composability**.

## Features

*   **Lazy Evaluation**: `prices | sma(10)` syntax using C++20 Ranges. Zero-allocation views.
*   **Policy-Based Storage**: Choose between `Dense` (Vector) or `Sparse` (Map) storage for Market Profiles.
*   **Auction Market Theory**: Native support for TPO and Volume Profile.
*   **Community Registry**: Git-based extensibility via the `contrib/` directory.

## Quick Start

### 1. Lazy Syntax (Recommended)
```cpp
#include "quanux/indicators/sma.hpp"

std::vector<double> prices = { ... };
auto sma_view = prices | quanux::indicators::sma(10);

for(auto val : sma_view) {
    if (!std::isnan(val)) { 
        // Signal logic...
    }
}
```

### 2. Traditional Syntax (Eager)
```cpp
auto results = quanux::indicators::compute_sma(prices, 10);
// returns std::vector<double>
```

### 3. Market Profile
```cpp
#include "quanux/indicators/market_profile.hpp"

// Use DenseStorage for speed on standard futures
quanux::indicators::MarketProfile<quanux::indicators::DenseStorage> profile(0.25);
profile.process(4500.25);
```

## System Integration

`libquanux-indicators` sits at the heart of the compute graph but remains a standalone module.

*   **Inputs**: accepts generic `std::ranges` (vectors, deques, memory mapped files).
*   **Outputs**: produces lightweight Views (lazy) or standard Vectors (eager).
*   **Bindings**: Python bindings enable the **Strategy Builder** UI to visualize these indicators without rewriting logic.

### Python Usage
```python
import quanux_indicators as qi
sma = qi.compute_sma([1.0, 2.0, 3.0], 2)
```


## Module Management

As a QuanuX Core Module, this library can be managed via the CLI.

**Uninstall (Clean Removal)**:
If you prefer a different indicator engine, you can strip this entirely:
```bash
quanuxctl module remove indicators
```
*This will remove the `server/indicators` directory and update the build system.*

**Re-Install**:
```bash
quanuxctl module install indicators
```

**Community Extensions**:
```bash
quanuxctl indicators install github.com/user/fancy-algos
```


## Adding Indicators

Place your header-only indicator libraries in `include/contrib/`.
Example: `include/contrib/my_strategy/super_rsi.hpp`

The CMake build system will automatically include these paths.
