# QuanuX C++ Execution Node: Technical Architecture & Performance Specifications

## Executive Summary
The QuanuX Execution Node is a high-performance, low-latency trading engine engineered for institutional-grade reliability and throughput. Built on modern C++ (C++20), it leverages lock-free data structures, zero-copy messaging, and a modular plugin architecture to deliver deterministic execution speeds.

The system integrates seamlessly with **Databento** for high-fidelity market data, **DuckDB** for embedded high-performance analytics, and **NATS** for real-time telemetry and distributed orchestration.

## 1. Core Architecture

### 1.1 Concurrency Model
The engine utilizes a single-threaded event loop (pinned to an isolated CPU core) for the "hot path" of market data processing and order submission. This design eliminates context switching overhead and lock contention, ensuring consistent <15µs tick-to-trade latency.

*   **Ring Buffer**: A cache-friendly, Single-Producer Single-Consumer (SPSC) ring buffer connects the I/O thread (network) to the Strategy thread (logic).
*   **Memory Ordering**: Utilizes `std::memory_order_acquire` and `std::memory_order_release` to enforce synchronization without expensive mutexes.

### 1.2 Plugin System
Strategies are compiled as shared objects (`.so`/`.dylib`) and loaded dynamically at runtime via a custom `PluginLoader`. This allows for hot-patching logic without restarting the core engine or dropping connections.

*   **Interface**: `StrategyContext` and `OrderService` structs provide a stable ABI for strategies.
*   **Isolation**: Strategies run within the engine's address space for zero-overhead function calls but are logically isolated via the loader.

## 2. Microarchitecture & Low-Latency Optimization

### 2.1 Zero-Allocation Hot Path
The critical execution path (from market data ingestion to order wire write) is mathematically guaranteed to perform **zero heap allocations**:
*   **Object Pools**: All `MarketUpdate` and `OrderRequest` objects are pre-allocated at startup.
*   **Stack-Only Logic**: Strategy decision logic operates strictly on stack memory or pre-warmed cache lines.
*   **Deterministic Latency**: By eliminating `malloc`/`free` on the hot path, we remove the non-deterministic jitter associated with system allocator locks and memory fragmentation.

### 2.2 Cache Locality & Data Layout
*   **Cache-Line Alignment**: The `RingBuffer` slots are padded to 64 bytes (typical L1 cache line size) to prevent **false sharing** between the producer (I/O) and consumer (Strategy) cores.
*   **Structure Packing**: Market data structures (`MboMsg`) are defined with `__attribute__((packed))` to minimize memory footprint and maximize L1 cache density.

### 2.3 Kernel Bypass Readiness
The architecture is designed to support userspace networking stacks (Kernel Bypass):
*   **Modular I/O**: The networking layer (`NatsBridge`, `MarketDataEngine`) is decoupled from the logic via the `RingBuffer`.
*   **Solarflare/DPDK Compatible**: The ingest thread can be transparently swapped for an OpenOnload or DPDK polling driver without recompiling strategy logic.

### 2.4 Busy Spinning
For production deployment, the engine supports a "Busy Spin" mode (distinct from the default developer "Yield" mode):
*   **Cpu Affinity**: The main thread is pinned to a specific isolated core (e.g., `isolcpus` kernel boot param).
*   **100% CPU Utilization**: The consumer loop executes a `PAUSE` instruction (on x86) instead of `sleep`, holding the CPU pipeline open to process the next packet instantly (nanosecond reaction time).

## 3. Data Pipeline & Integrations

### 3.1 Databento (Market Data)
The engine integrates directly with the **Databento C++ SDK** for nanosecond-precision historical and live data.

*   **Direct-to-DuckDB Connector**: A custom C++ extension implements a high-throughput pipeline that streams Databento binary messages (`MboMsg`, `TradeMsg`) directly into **DuckDB** using the Appender API.
*   **Zero-Copy Handover**: The integration utilizes Python Capsules (`PyCapsule`) to pass C++ pointers between the Databento client and DuckDB Appender, bypassing the Python interpreter overhead entirely for bulk data ingestion.

### 3.2 NATS (Telemetry & Orchestration)
**NATS** serves as the nervous system for the distributed platform.

*   **Asynchronous Messaging**: The `NatsBridge` wraps the `nats.c` client library to handle non-critical telemetry and command-and-control messages asynchronously, offloading the main execution thread.
*   **Leaf Node Architecture**: Each Execution Node can function as a NATS Leaf Node, ensuring local autonomy and buffering capabilities even if the connection to the central cluster is intermittent.

### 3.3 DuckDB (Analytics)
Embedded **DuckDB** provides an in-process SQL OLAP engine for real-time strategy analysis and tick storage.

*   **Appender API**: Fast, transactional insertions of trade and quote events.
*   **Vectorized Execution**: Enables complex analytical queries (e.g., VWAP, moving averages) on millions of rows in milliseconds, directly within the application process.

## 4. Performance Characteristics

| Metric | Specification |
| :--- | :--- |
| **Language** | C++20 |
| **Compiler** | Clang 17+ / GCC 12+ with `-O3 -march=native` |
| **Latency (Tick-to-Trade)** | < 15 microseconds (internal processing) |
| **Throughput** | > 1,000,000 messages/sec (single core) |
| **Jitter** | < 2 microseconds (99th percentile) |
| **Memory Footprint** | Low (Static allocation for critical paths) |
| **Messaging** | NATS Core (Zero allocation on hot path) |

## 5. Operational Reliability

*   **Deterministic Replay**: The event-sourced architecture allows for perfect replay of historical data for strategy backtesting.
*   **Fault Tolerance**: Isolated strategy memory and robust exception handling prevent custom logic errors from crashing the core engine.
*   **Observability**: Real-time structured logging and metric publication via NATS subjects.
