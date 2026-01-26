---
description: Definitive guide to the QuanuX High-Performance Backtesting Engine (C++20/Python).
---

# QuanuX Backtesting Engine

## Overview
The **QuanuX Backtesting Engine** is a high-performance, C++20 simulation framework designed for:
1.  **Speed**: Millions of events per second (fast-forward mode).
2.  **Accuracy**: Event-driven architecture with precise market replay.
3.  **Flexibility**: Supports "Unix Philosophy" via pipe streaming and NATS-based real-time replay.
4.  **Hybrid Workflow**: Core logic in C++, analysis/scripting in Python.

## Architecture

### 1. Dual-Mode Simulation
*   **Fast Forward**: Runs as fast as CPU allows. Best for parameter optimization and statistical validation.
*   **Real-Time Replay (NATS)**: Replays historical data at wall-clock speed (or accelerated factor) and publishes to NATS.
    *   **Purpose**: Allows you to connect a frontend or live trading bot to the "past" as if it were happening now.
    *   **Flag**: `--nats-replay`

### 2. Data Feeders
*   **DuckDB Feeder**: Queries `*.parquet` or `*.csv` files efficiently using DuckDB.
*   **DBN Pipe Feeder**: Reads raw **Databento Binary Encoding (DBN)** from `stdin`.
    *   **Usage**: `dbn-cli get range ... | ./quanux_backtest --stdin`
    *   **Benefit**: No intermediate disk storage; streams terabytes of data directly from Databento servers to the engine.

### 3. Common Library
*   **Location**: `QuanuX-Common/`
*   **Shared Components**: `OrderBookL3`, `StrategyInterface`.
*   **Consistency**: Ensures the `OrderBook` logic used in backtesting is *identical* to the live `execution-node`.

## Metrics Engine (`PerformanceAnalyzer`)
The engine features a single-pass, O(n) metrics calculator using Welford's Algorithm regarding variance/standard deviation, ensuring numerical stability.

### Available Metrics
#### Profitability
*   **Net Profit**: Total PnL.
*   **Profit Factor**: Gross Profit / Gross Loss.
*   **Expectancy**: Average dollar amount per trade.
*   **CAGR**: Compound Annual Growth Rate.

#### Risk & Drawdown
*   **Max Drawdown (MDD)**: Largest peak-to-valley decline (%).
*   **Ulcer Index**: Measure of downside volatility (root mean square of drawdowns).
*   **Serenity Index**: Risk-adjusted return metric (TBD).

#### Ratios
*   **Sharpe Ratio**: Excess return per unit of total volatility.
*   **Sortino Ratio**: Excess return per unit of *downside* volatility.
*   **Omega Ratio**: Probability weighted ratio of gains vs losses.
*   **Calmar Ratio**: CAGR / Max Drawdown.
*   **SQN (System Quality Number)**: Van Tharp's metric for system expectancy stability.

### Python Integration (`quanux_backtest`)
We provide a high-level Python package that wraps the C++ engine and integrates with Pandas.

*   **Package**: `quanux_backtest` (located in `python/`)
*   **Usage**:
    ```python
    import pandas as pd
    from quanux_backtest import BacktestAnalyzer
    
    # 1. Load Data
    equity_curve = pd.Series([...])
    
    # 2. Analyze (C++ Backend)
    analyzer = BacktestAnalyzer(start_equity=10000.0)
    metrics = analyzer.process_equity(equity_curve)
    
    print(f"Sharpe: {metrics['sharpe_ratio']}")
    print(f"Ulcer Index: {metrics['ulcer_index']}")
    ```

## Extending the Engine

### How to Add a New Metric
1.  **Update Struct**: Add the field to `struct Metrics` in `generated/include/engine/metrics/PerformanceAnalyzer.h`.
2.  **Update Logic**: Add the calculation code in `PerformanceAnalyzer::calculateMetrics()`.
    *   *Tip*: If it requires time-series variance, use the `WelfordAccumulator`.
3.  **Update Python Bindings**: Add `.def_readonly(...)` to `src/bindings/bindings.cpp` so Python can see it.
4.  **Recompile**: Run `./build.sh` or `cmake --build build --target quanux_metrics`.

## Agent Instructions
*   **To Run Backtest**:
    *   Look for `quanux_backtest` executable in `QuanuX-Backtesting-Engine/cpp/build/`.
    *   Command: `./quanux_backtest --symbols "ESZ4" --strategy "PingPong"`
*   **To Use Pipe Feeder**:
    *   Ensure `dbn-cli` is installed.
    *   Command: `cat my_data.dbn | ./quanux_backtest --stdin`
*   **To Verify Bindings**:
    *   Run `python verify_bindings.py` in the root of the backtester directory.

