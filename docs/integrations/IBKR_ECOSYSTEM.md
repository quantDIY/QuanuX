# QuanuX Interactive Brokers Ecosystem Architecture

> [!IMPORTANT]
> This document details the **Tri-Pillar Architecture** for IBKR integration, designed for High-Frequency Trading (HFT), Institutional Access, and Polyglot Strategy Development.

## 1. The Tri-Pillar Design
We do not rely on a single API. Instead, we provide specialized extensions for specific trading needs.

| Extension | Protocol | Primary Use Case | Runtime | Performance |
| :--- | :--- | :--- | :--- | :--- |
| **`ibkr_fix`** | FIX 4.2 | **Execution & Order Routing** | C++ (QuickFIX) | Ultra-Low Latency |
| **`tws_api`** | Native TCP | **Market Data & Algos** | C++ / Python | Low Latency |
| **`ibkr_web`** | REST | **Portfolio & Admin** | Python | High Latency |

## 2. Cross-Platform Readiness (Linux & Windows)
The ecosystem is engineered for both trading servers (Linux) and developer workstations (Windows).

-   **Linux**:
    -   Builds with `gcc`/`clang`.
    -   Links against `pthread`.
    -   Standard production environment.
-   **Windows**:
    -   Builds with `MSVC`.
    -   Automatically links `ws2_32` (Winsock) via CMake.
    -   Supports `mimalloc` override for performance.
    -   Ideal for local testing with TWS Workstation.

---

## 3. High-Performance "Turbo" Mode
The `ibkr_fix` extension supports a unique **"Enhance"** capability, allowing traders to recompile the adapter with specialized memory allocators and logging strategies to shave microseconds off the critical path.

### The `quanuxctl enhance` Command
This command triggers a recompilation of the C++ adapter with specific flags.

#### A. Memory Allocators
-   **`system`**: Standard `malloc/free`.
-   **`jemalloc`** (Facebook): Best for multi-threaded environments, reducing fragmentation.
-   **`mimalloc`** (Microsoft): Optimized for small object allocation, common in FIX message parsing.

#### B. Logging Strategies
-   **`file`**: Synchronous. Every log writes to disk. Safe but blocks the execution thread.
-   **`async`**: Asynchronous Ring Buffer (`spdlog`). Writes to a memory buffer; a separate thread flushes to disk. Zero-latency impact on trading path.
-   **`null`**: Disables logging entirely for maximum speed.

---

## 3. Centralized SDK Injection
To support proprietary or external SDKs without bloating the repo or violating licenses, we use a **Centralized Injection** model.

**Location**: `extensions/sdks/`

### Workflow
1.  User downloads SDK (e.g., TWS API, OnixS).
2.  User runs `quanuxctl integrate <name> --path <local_path>`.
3.  The CLI copies the SDK to `extensions/sdks/<name>`.
4.  Extensions link against this path relative to their `CMakeLists.txt`.

**Benefits**:
-   **Decoupled**: Extensions are thoroughly tested code; SDKs are swappable blobs.
-   **Persistent**: SDKs survive extension updates.

---

## 4. Polyglot Architecture
We adhere to **Rule 0 (Performance = C++)** while enabling Python strategy development.

### The Wrapper Pattern
-   **Core**: The `tws_api` adapter is written in C++ (`TwsAdapter.cpp`) and implements the `IExecutionProvider` interface.
-   **Binding**: We use **Pybind11** to expose this C++ class as a Python module (`TwsApi.so`).
-   **Strategy**: Python code imports `TwsApi`, instantiates the C++ adapter, and uses it directly. There is no "Python Logic" layer slow-down; Python acts as a high-level controller for the C++ engine.

## 5. External Library Hooks
Advanced users can link their own math libraries (Intel IPP, CUDA, Eigen) into the build system.
-   **File**: `extensions/cpp/<ext>/user_libs.cmake`
-   **Mechanism**: The CMake build script checks for this file and includes it.
-   **Usage**: Add `find_package(...)` and `target_link_libraries(...)` to inject your custom algo logic directly into the engine binary.
