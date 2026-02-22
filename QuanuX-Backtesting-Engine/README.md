# QuanuX Crucible (Backtesting Engine)

QuanuX Crucible is a hyper-advanced, ultra-low-latency backtesting simulation engine built in C++20. It executes Python and Cython trading strategies at speeds approaching 100x that of traditional backtesters by bypassing standard memory copies, SQL overhead, and Python object serialization.

## Core Architecture

### 1. `alignas(128) CrucibleTrade`
Execution metric arrays are meticulously aligned to 128 bytes. This guarantees perfect CPU cache line saturation, preventing false sharing and latency spikes when executing bulk data manipulations.

### 2. DuckDB C++ Appender (`DuckDBFeeder`)
Crucible does not parse SQL. It utilizes the native `duckdb::Appender` API to inject vectorized blocks of `CrucibleTrade` structures directly into DuckDB's in-memory columnar engine.

### 3. Cython Memory Linking
The gap between the `duckdb::Appender` and Python algorithms is bridged exclusively via Cython `.pxd`/`.pyx` wrappers. The Engine is packaged into `libbacktest_engine.a` and statically linked into `quanux_backtest.cython.quanux_crucible.so`, meaning Python developers gain C++ throughput transparently.

## Usage (Human Developers)

The engine's lifecycle is orchestrated entirely by `quanuxctl`:

### Starting a Sandbox
```bash
quanuxctl crucible start my_strategy --version 1.0.0
```
This detaches the Python simulation from the terminal via `os.setsid()` and executes the simulation securely in the background, logging its PID to `/tmp/quanux_crucible.pid`.

### Monitoring the Engine
```bash
# Check CPU/Memory Load
quanuxctl crucible status

# Watch NATS live Telemetry
nats sub "sys.crucible.report.my_strategy"
```

### Pulling Metrics (High-Speed API)
```bash
quanuxctl crucible report my_strategy
```
This completely bypasses the Python pandas pipeline. It loads the compiled Cython extension and uses the C++ `get_metrics_json` bound method to rip the instantaneous metrics payload out of the `.duckdb` state file at C++ speeds.

## L3 Execution Metrics (Forensics)
Unlike basic backtesters, Crucible enforces institutional-grade L3 (Market By Order) metric collection natively:
*   **Latency Slippage (bps)**: Measured automatically based on execution delay.
*   **Queue Position at Entry**: Determined by simulated volume-ahead tracking in the `FifoMatcher`.
*   **MAE/MFE**: Maximum Adverse/Favorable Excursion mathematically tracked per tick.
