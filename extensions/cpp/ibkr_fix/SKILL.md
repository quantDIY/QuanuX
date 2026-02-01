---
name: ibkr_fix_adapter
description: Ultra-Low Latency FIX Adapter for Interactive Brokers with 'Turbo' capabilities.
---

# IBKR FIX Adapter (Turbo Edition)

This is the **Execution Workhorse** of the QuanuX-IBKR Ecosystem. It provides institutional-grade connectivity via the FIX protocol, enhanced with QuanuX performance tuning.

## 🚀 Turbo Mode (`quanuxctl enhance`)
This extension supports **Performance Enhancements** that can be triggered via the CLI. This recompiles the adapter with specialized memory allocators and logging strategies.

### Command
```bash
quanuxctl enhance ibkr_fix --allocator=mimalloc --logger=async
```

### Options
1.  **Allocators**:
    -   `system`: Default OS malloc. Safe, standard.
    -   `jemalloc`: (Facebook) Excellent for multi-threaded fragmentation reduction.
    -   `mimalloc`: (Microsoft) Extremely fast, superb for high-frequency object creation/destruction.
2.  **Loggers**:
    -   `file`: Standard synchronous file logging (Blocking I/O). Safe but slow.
    -   `async`: Ring-buffer logging (`spdlog`). Non-blocking. Critical for high-throughput strategies.
    -   `null`: Disables all FIX message logging. Maximum speed, zero visibility.

## 🔌 External Libraries (`user_libs.cmake`)
Need to link a proprietary math library or hardware accelerator?
Edit `extensions/cpp/ibkr_fix/user_libs.cmake`:
```cmake
find_package(IntelIPP REQUIRED)
target_link_libraries(ibkr_fix_adapter PRIVATE ntelIP::ipp)
```
The build system automatically picks this up during `enhance` or `install`.

## Configuration
edit `config/ibkr.cfg` for your Gateway credentials.
-   **Dictionary**: Uses `config/FIX42-IBKR.xml` (Auto-generated custom tags like `6035`).

## Usage
Link against `ibkr_fix_adapter` in your Execution Node or use standard QuickFIX bindings pointing to this config.
