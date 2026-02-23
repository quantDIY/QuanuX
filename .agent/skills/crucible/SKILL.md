---
name: QuanuX Crucible
description: How to operate the C++ Aligned-Memory Backtester from Python (quanuxctl).
---

# QuanuX Crucible Skill

## Overview
The **QuanuX Crucible** is a state-of-the-art C++20 simulation environment. It executes strategies and models Limit Order Book (L3) dynamics at speeds 100x faster than traditional Python execution stacks. It achieves this by bypassing the Python interpreter for data ingestion and metric tracking, binding directly to DuckDB's C++ Appender API via Cython.

## AI & Agent Instructions

If a user requests backtesting, execution telemetry, or strategy simulation, you **must use Crucible** and orchestrate it via the `quanuxctl crucible` command group. 

DO NOT attempt to run standard python scripts (e.g. `python my_strategy.py`) if the user mentions "Backtesting in Crucible". The process must be isolated using the CLI.

### 1. Generating a Crucible Harness
The test harness must have been generated previously via the Foundry generator, outputting to `/server/backtests/<strategy_name>_v<version>/crucible_harness_python.py`. 
If it does not exist, use the Foundry `forge` tools to create one.

### 2. Starting execution
```bash
quanuxctl crucible start <strategy_name> --version <version>
```
*   This detaches the Python simulation and runs it via `os.setsid()`. 
*   It logs the process ID to `/tmp/quanux_crucible.pid`.
*   Crucible writes metric chunks directly into `/server/backtests/<strategy_v1.0.0>/crucible.duckdb`.

### 3. Monitoring load
Provide the user with live CPU/Memory telemetry:
```bash
quanuxctl crucible status
```

### 4. Fetching the Results (Cython Extractor)
To pull the data without halting the active agent context, execute the instantaneous `report` query.
```bash
quanuxctl crucible report <strategy_name> --version <version>
```
**CRITICAL**: `quanuxctl crucible report` does not build a Pandas dataframe or fire up a SQL runtime. It calls the `get_metrics_json()` Cython binding directly mapped to DuckDB's native C API and returns a raw JSON payload of the backtest. 

If this query fails (e.g. "No backtest data found"), you must `start` the engine first.

### Data Architecture Restrictions
*   When manipulating the `CrucibleTrade` class internally (via Python binding), remember that its underlying C++ struct (`alignas(128)`) is strictly padded to exactly 128 bytes to hit CPU L1/L2 cache lines efficiently.
*   Do not inject pure Python attributes into the `PyCrucibleTrade` class. Fields must exist in `quanux_crucible.pxd`.
