# QuanuX Execution Node (Edge Supervisor)

The **Execution Node** is a bare-metal supervisor designed for low-latency trading. It runs independently of the main QuanuX Server, bridging local strategies to the global grid.

## The Dual-Engine Philosophy

QuanuX offers two robust paths for strategy execution:

### Option A: Python / Go (Flexibility)
*   **Engine**: Go Supervisor running Python subprocesses.
*   **Best For**: Machine Learning (PyTorch/TensorFlow), Anaconda ecosystem users.
*   **Details**: Strategies run in a managed environment using `quanux_indicators` bindings.

### Option B: C++ Native (Speed)
*   **Engine**: C++20 Native Binary.
*   **Best For**: High-Frequency Trading (HFT).
*   **Details**: Strategies link directly against the core for zero-overhead execution.

**Why this works**:
Both options use the **same underlying C++ indicator library**. You can choose the runtime that fits your trading style without sacrificing signal accuracy.

## Architecture
*   **Core**: C++20 Engine (`cpp/`) or Go Runtimes (`cmd/`).
*   **Transport**: ZMQ (Hot Path) + NATS (Warm Path).
*   **Storage**: DuckDB (Local History).

## Usage
Deploy usage instructions coming soon via `quanuxctl`.
