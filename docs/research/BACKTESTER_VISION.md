# QuanuX Backtester Vision (Issue #17)

## Executive Summary
Existing open-source backtesters (Lean, NautilusTrader) force a trade-off: **Ease of Use** (Python) vs **Raw Speed** (C++/Rust). Most also suffer from a "Simulation Gap" where the backtest engine logic differs from the live execution logic.

**QuanuX** will solve this by using the **exact same C++ binary** for both Live Trading and Backtesting. The "Backtester" effectively becomes a "Time Machine" wrapper around the live engine.

## Competitive Analysis

| Feature | QuantConnect (Lean) | NautilusTrader | QuanuX (Proposed) |
| :--- | :--- | :--- | :--- |
| **Core Language** | C# | Rust | **C++20** |
| **Strategy Lang** | C# / Python (slow) | Python / Cython | **C++ (Native) / Python (Zero-Copy)** |
| **Data Engine** | File-based / Custom | Parquet / Catalog | **DuckDB (Vectorized SQL)** |
| **Live Parity** | High (same engine) | High (same engine) | **Perfect (Same Binary .so)** |
| **Queue Sim** | Basic | Basic | **L3 Microstructure Zoom** |

## The QuanuX Advantage

### 1. The "Zero-Port" Guarantee
The Strategy is a compiled `.so`. The simple `PluginLoader` doesn't care if the `MarketUpdate` came from a live socket or a DuckDB row. It is binary-identical.

### 2. DuckDB as the Time Drive
*   **Vectorized Loading**: DuckDB loads millions of ticks/sec into memory.
*   **Zero-Copy Handover**: We pass pointers from DuckDB's Arrow result directly to the Engine's C++ structures. 0% serialization overhead.

### 3. NATS "Ghost" Replay
The backtester optionally publishes to a **NATS** subject. Only the Risk Engine or UI listens. This allows for "Full System Visualization" without risking real capital.

---

## "Eureka" Features (Advanced HFT Simulation)

### 4. Microstructure "Zoom" & L3 Queue Simulation
*Concept: Standard backtesters assume "if price touched X, I filled." HFT does not.*

1.  **Macro Scan**: Engine runs fast on OHLCV bars to find entry triggers.
2.  **Micro Zoom**: Upon trigger, the engine locally rewinds and loads high-fidelity **L3 (MBO)** data from Databento for that specific +/- 1 minute window.
3.  **Queue Position Simulation**:
    *   The engine estimates "Wire Latency" (e.g., 5ms).
    *   It inserts the specific Order ID into the virtual L3 book at the correct timestamp.
    *   It simulates the matching engine logic (FIFO vs Pro-Rata) to determine if the order *actually* filled before the price moved away or the level was canceled.
4.  **Visual Replay**: The Quant can watch a "Replay" of the Order Book (DOM) for that specific trade to see the liquidity battle.

### 5. Latency Sensitivity Heatmaps
*Concept: "Does my alpha survive if I live in London instead of NY within the same datacenter?"*

The simulator runs the *same* trade window 100 times, injecting randomized jitter (10us, 100us, 1ms, 5ms... 50ms).
*   **Output**: A heatmap showing at what latency threshold the strategy becomes unprofitable.
*   **Use Case**: Tells the CTO "Do not deploy this unless we have <2ms roundtrip."

### 6. Shadow Mode (Parallel Universe)
*Concept: "Is my model drifting?"*

In Production, the Execution Node runs **Two Threads**:
1.  **Live**: Real money, real executions.
2.  **Shadow**: Same feed, same logic, but holds the trades in memory.
3.  **Diff Engine**: Real-time alerts if the Live trade price diverges significantly from what the Model *thought* it would get.

### 7. Synthetic Stress Injection
*Concept: "What if a flash crash happens NOW?"*

Inject synthetic "Black Swan" L3 events (liquidity pull, massive sweep) into the replay stream to test if the strategy handles the chaos gracefully or crashes.

---

## Architecture

```mermaid
graph TD
    DB[(DuckDB / Parquet)] -->|Zero-Copy Arrow Batch| Injector[C++ Market Data Injector]
    Injector -->|Sequenced Ticks| RingBuffer
    RingBuffer --> Strategy[Strategy .so]
    Strategy -->|Orders| Matcher[Simulated Exchange]
    Matcher -->|Fills| RingBuffer
    
    subgraph "L3 Microstructure Simulation"
        Matcher -.->|Request Detail| DB
        DB -.->|L3 MBO Data| Matcher
        Matcher -->|Queue State| QueueSim[Queue Position Logic]
    end
    
    subgraph "Optional Side Effects"
        Strategy -.->|Telemetry| NATS[NATS "Ghost" Stream]
        NATS -.-> UI[Front End]
    end
```
