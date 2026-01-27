---
name: manage-quanux-statistics
description: Guide for developing and operating the QuanuX Statistics & Research Node.
---

# QuanuX Statistics Node

The **QuanuX Statistics Node** (`quanux_stats`) is a purpose-built service for real-time market analysis and signal generation. It bridges the gap between raw market data and high-level strategy logic by providing pre-calculated metrics.

## Capabilities

1.  **Online Statistics**:
    -   Uses **Welford's Algorithm** to track Variance and Standard Deviation with $O(1)$ complexity/storage per tick.
    -   Tracks **Z-scores** relative to a rolling window (default 100 ticks).
    -   Calculates **Pairwise Correlation** matrices in real-time.

2.  **Data Persistence**:
    -   Ingests `MARKET.*` data from NATS (JSON format).
    -   Writes to **DuckDB** (`market_stats.duckdb`) for offline research and backtesting.
    -   *Future*: Will support Parquet export for S3 archival.

3.  **Signal Publishing**:
    -   Publishes derived metrics to `STATS.<SYMBOL>` on NATS.
    -   Format: `{"symbol": "ES", "volatility": 12.5, "z_score": 1.2, ...}`

## Architecture

-   **Language**: C++20
-   **Core**: `StatsEngine` class.
-   **Storage**: `stats_map_` (Red-Black Tree) for per-symbol state.
-   **Concurrency**: Single-threaded NATS consumer (for now) protected by `std::mutex` for future expansion.

## Development Guide

### Adding a New Metric
1.  **Update `InstrumentStats`** in `include/stats_engine.h`:
    Add a new accumulator (e.g., `double sum_log_return`).
2.  **Update `update()`** in `src/stats_engine.cpp`:
    Implement the online update formula.
3.  **Publish**:
    Add the new field to the `json derived` object in the NATS callback.

### Running Locally
```bash
# Build
cd QuanuX-Statistics/cpp/build
make quanux_stats

# Run (Ensure NATS is up)
./quanux_stats
```

## Dependencies
-   **DuckDB**: Embedded DB for storage.
-   **NATS C Client**: High-performance messaging.
-   **Eigen**: Linear algebra (Matrix operations).
-   **nlohmann/json**: Serialization.
