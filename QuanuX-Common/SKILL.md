---
description: QuanuX Common Library - Shared C++ Core
---

# QuanuX Common Library

## Overview
**QuanuX-Common** contains the shared C++ components used by both the live **Execution Node** and the **Backtesting Engine**.
It ensures ABI (Application Binary Interface) stability and consistent data structures across the ecosystem.

## Core Components
The library resides in `cpp/include/quanux/common/`.

### 1. `StrategyInterface.h`
*   **Purpose**: Defines the Pure C ABI for trading strategies.
*   **Key Structs**:
    *   `MarketUpdate`: Tick data layout.
    *   `Signal`: Output from signal modules.
    *   `OrderRequest`: Structure for submitting orders.
    *   `Strategy`: V-Table for strategy plugins.
*   **Namespace**: `quanux::common` (wrapped in `extern "C"` for symbol stability).

### 2. `OrderBookL3.h`
*   **Purpose**: High-performance Order Book (Level 3 / Market-By-Order).
*   **Features**:
    *   O(1) Order ID lookup (std::unordered_map).
    *   Sorted Price Levels (std::map).
    *   Shared logic for both Backtester (replaying ticks) and Execution Node (live processing).

## Developer Guidelines
1.  **Do Not Duplicate**: If a data structure is needed by more than one component (e.g., Node + Backtester), place it here.
2.  **ABI Stability**: Changes to `StrategyInterface.h` require recompiling ALL strategy plugins. Avoid changing existing struct layouts if possible.
3.  **Include Path**: Add `QuanuX-Common/cpp/include` to your `CMakeLists.txt` include directories.
    ```cmake
    include_directories(${CMAKE_SOURCE_DIR}/../../QuanuX-Common/cpp/include)
    ```
4.  **Usage**:
    ```cpp
    #include <quanux/common/StrategyInterface.h>
    using namespace quanux::common;
    ```
