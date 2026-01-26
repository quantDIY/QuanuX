---
name: universal_runner
description: Instructions for operating the QuanuX Universal Runner (quanux_runner) for Live Trading and Offline Backtesting.
---

# Universal Runner Operator Guide

The `quanux_runner` is the core C++ execution engine for QuanuX. Use this skill to execute strategies, run backtests, or deploy to live markets.

## 1. Capabilities
*   **Offline Replay**: High-performance replay of Databento `.dbn` files (MBO/L3 supported).
*   **Rithmic Replay**: Historical tick replay via RApi+ (Trade info).
*   **Live Trading**: Direct connection to Databento Live API via `libdatabento` or Rithmic RApi+.
*   **Zero-Copy Execution**: Loads strategies as shared objects (`.dylib`/`.so`) for maximum speed.

## 2. Usage

### Location
Binary is located at: `execution-node/cpp/build/quanux_runner`

### CLI Arguments
| Argument | Required | Description |
| :--- | :--- | :--- |
| `--strategy <path>` | **Yes** | Path to the compiled strategy shared object (`.dylib`/`.so`). |
| `--file <path>` | No* | Path to input `.dbn` file (Offline Mode). *Required for Backtesting.* |
| `--key <key>` | No* | Databento API Key. *Required for Live Mode.* |
| `--symbol <sym>` | No | Symbol to subscribe to (default: `ES`). |
| `--dataset <ds>` | No | Dataset (default: `GLBX.MDP3`). |
| `--ruser <u>` | No* | Rithmic Username. *Required for Rithmic.* |
| `--rpass <p>` | No* | Rithmic Password. *Required for Rithmic.* |
| `--rexch <e>` | No | Rithmic Exchange (e.g., `CME`). *Required for Rithmic.* |
| `--start <ts>` | No | Start Timestamp (Unix Epoch). *Required for Replay.* |
| `--end <ts>` | No | End Timestamp (Unix Epoch). *Required for Replay.* |

### Examples

#### A. Run Offline Backtest (Recommended)
Use this to verify strategy logic using historical data. Zstd decompression is handled automatically if the runner was linked correctly.
```bash
./execution-node/cpp/build/quanux_runner \
    --strategy execution-node/cpp/build/demo_strategy.dylib \
    --file data/raw/glbx-mdp3-20260122.mbo.dbn
```

#### B. Run Live Mode
Requires `DATABENTO_API_KEY` (env var or arg).
```bash
export DATABENTO_API_KEY=db-xc...
./execution-node/cpp/build/quanux_runner \
    --strategy execution-node/cpp/build/demo_strategy.dylib \
    --symbol ESH6
```

#### C. Run Rithmic Replay
Replay historical trades from Rithmic servers.
```bash
./execution-node/cpp/build/quanux_runner \
    --strategy execution-node/cpp/build/demo_strategy.dylib \
    --ruser "DEMO_USER" \
    --rpass "DEMO_PASS" \
    --symbol "ES.M24" \
    --rexch "CME" \
    --start 1715000000 \
    --end 1715003600
```

## 3. Workflow Checklist

Before running, ensure:
1.  **Strategy Built**: The strategy must be compiled freshly.
    *   `cmake --build execution-node/cpp/build --target <strategy_name>`
2.  **Runner Built**: The host binary must be up to date.
    *   `cmake --build execution-node/cpp/build --target quanux_runner`
3.  **Data Ready**: For offline mode, ensure the `.dbn` file exists and is readable.

## 4. Troubleshooting

*   **Exit Code 139 (Segfault)**:
    *   Usually means the strategy is crashing (e.g., indicator out of bounds).
    *   **Fix**: Add `std::cout` logs to `on_market_data` in the strategy and rebuild to isolate the crash.
*   **"Symbol not found"**:
    *   The strategy `.dylib` is missing symbols or ABI mismatch.
    *   **Fix**: Ensure `extern "C" { Strategy* create_strategy() ... }` is strictly defined in the strategy source.
*   **"Invalid argument 'key'"**:
    *   API Key is missing or invalid in Live Mode.
