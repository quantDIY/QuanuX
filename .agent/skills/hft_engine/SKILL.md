---
name: HFT Stats Engine
description: Usage and architecture of the QuanuX High-Frequency Trading Statistics Engine.
---

# HFT Stats Engine Skill

## Overview
The **QuanuX Stats Engine** is a C++20 microservice designed for sub-microsecond signal generation. It ingests binary market data, computes rolling statistics (Welford's algorithm), and emits trading signals via a lock-free queue.

## Capabilities
1.  **Binary Ingestion**: Consumes `MARKET.BIN` NATS subjects. Payload must be a 64-byte `quanux::MarketTick` struct.
2.  **Persistence**: High-throughput storage to DuckDB via Appender API (no SQL overhead).
3.  **Signaling**: Emits `Signal` structs to an internal SPSC queue when Z-Score > 2.0.
4.  **Telemetry**: Logs correlation matrices to stdout every 10 seconds.

## Architecture
-   **Thread A (Producer)**:
    -   Ingest NATS message (Zero-Copy cast).
    -   Append to DuckDB.
    -   Update Rolling Stats (RingBuffer).
    -   Push to SPSC Queue (Lock-Free).
-   **Thread B (Consumer)**:
    -   Spin-wait on SPSC Queue using `_mm_pause()`.
    -   Execute Strategy Logic (currently simulated).

## Data Structure (`quanux::MarketTick`)
Must be exactly **64 bytes** (Cache Line Aligned).
```cpp
struct alignas(64) MarketTick {
    uint64_t local_rec_ts;       // 8 bytes
    uint64_t exchange_ts;        // 8 bytes
    double price;                // 8 bytes
    uint32_t size;               // 4 bytes
    uint32_t flags;              // 4 bytes
    uint32_t instrument_id;      // 4 bytes
    // Implicit Padding: 4 bytes here
    uint64_t internal_arrival_ts;// 8 bytes
    uint64_t processing_start_ts;// 8 bytes
    uint8_t _pad[8];             // 8 bytes Explicit Padding
};
```

## Performance Benchmarks (macOS M1/M2)
-   **Min Latency**: 59 nanoseconds
-   **Avg Latency**: ~250 microseconds (OS scheduling noise)
-   **Throughput**: >3.2 Million msg/sec

## Usage
### Running the Engine
```bash
./quanux_stats
```

### Verifying
Use the provided Python script to send compliant binary packets:
```bash
python3 verify_hft_engine.py
```

### Benchmarking
Run the micro-benchmark to measure internal SPSC latency:
```bash
./benchmark_hft_engine
```
