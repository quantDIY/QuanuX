---
name: QuanuX HFT Execution Node
description: Ultra-low latency C++ execution engine ("Hyper-Node") for high-frequency trading strategies.
---

# QuanuX HFT Execution Node ("Hyper-Node")

## Overview
This directory (`execution-node/cpp`) contains the source code for the QuanuX High-Frequency Trading (HFT) Execution Engine. It is designed to run on bare-metal servers (Linux) with kernel-bypass networking and thread-per-core isolation to achieve nanosecond-level latency.

**Key Characteristic: Inverted Data Flow**
Unlike traditional system where the server streams data to the node, the **Hyper-Node** connects *directly* to the exchange (or FPGA) and streams market data *back* to the QuanuX Grid via NATS.

## Architecture

```mermaid
graph TD
    Exchange[Exchange / FPGA] -->|Direct Feed (C++)| MDE[MarketDataEngine]
    MDE -->|Ring Buffer (Nanoseconds)| Strategy[Strategy Engine]
    MDE -->|Async| Nats[NatsBridge]
    Strategy -->|Signals| OG[Order Gateway]
    Strategy -->|OrderUpdate| Nats
    Nats -->|market.data.*| Grid[QuanuX Grid / UI]
    Nats -->|node.telemetry.*| Risk[Risk Manager]
```

### 1. Core Components

#### `MarketDataEngine` (`src/market_data_engine.cpp`)
- **Role**: The "Mouth" of the node.
- **Function**: Loads C++ Extensions (e.g., `quanux_rithmic`, `quanux_databento`) dynamically.
- **Performance**:
    - **Hot Path**: Pushes `MarketUpdate` structs to a lock-free SPSC (Single-Producer Single-Consumer) Ring Buffer for the Strategy.
    - **Cold Path**: Pushes updates to `NatsBridge` for the UI/Grid.

#### `Engine` (`src/engine.cpp`)
- **Role**: The "Brain".
- **Design**: Thread-per-Core architecture.
- **Event Loop**: Uses `io_uring` (on Linux) for non-blocking I/O. 
- **Isolation**: Pinned to specific CPU cores (using `pthread_setaffinity_np`) to avoid OS context switches.

#### `Strategy` (`include/strategy_interface.h`)
- **Role**: The "Logic".
- **Interface**: Pure C ABI (`extern "C"`) for maximum stability and compatibility.
- **Cartridges**: Strategies are compiled as shared objects (`.so`) and loaded at runtime.
- **Callbacks**:
    - `on_market_data(ctx, update)`: Called on every tick.
    - `on_order_update(ctx, update)`: Called on fills/acks.
    - `on_signal(ctx, signal)`: (Optional) External signals.

#### `NatsBridge` (`src/nats_bridge.cpp`)
- **Role**: The "Reverse Pipe".
- **Thread**: Runs on a separate "Sidecar" thread (isolated from the Strategy core).
- **Channels**:
    - `market.data.<symbol>`: Live ticker stream for the Grid.
    - `node.telemetry.fills`: Real-time fill reports and PnL.
    - `node.heartbeat`: System health.

## Building and Running

### Prerequisites
- CMake 3.20+
- C++20 Compiler (GCC/Clang)
- Linux (Recommended for `io_uring` and CPU pinning) or macOS (Dev).

### Build Command
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

The build automatically fetches dependencies (NATS C client) via `FetchContent`.

## Writing a Strategy

Strategies implement the interface defined in `include/strategy_interface.h`.

**Example `my_strategy.cpp`**:
```cpp
#include "strategy_interface.h"
#include <iostream>

struct StrategyContext {
    int position;
};

void on_market_data(StrategyContext* ctx, const MarketUpdate* update) {
    if (update->price > 100 && ctx->position == 0) {
        // Buy Logic
    }
}

extern "C" Strategy* create_strategy() {
    static Strategy s = {
        .name = "MomentumAlpha",
        .on_market_data = on_market_data,
        // ...
    };
    return &s;
}
```
