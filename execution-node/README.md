# QuanuX Execution Node (Edge Supervisor)

The **Execution Node** is a bare-metal supervisor designed for low-latency trading. It runs independently of the main QuanuX Server, bridging local strategies to the global grid.

## The Dual-Engine Philosophy (Graduation)

QuanuX supports a unique "Graduation" workflow for strategies:

### Phase 1: Research & Prototyping (Python)
*   **Where**: Strategy Builder / Jupyter.
*   **Engine**: Python (Pandas/PyTorch).
*   **Indicators**: `import quanux_indicators` (Python Bindings).
*   **Goal**: Rapid iteration, machine learning integration, visual debugging.

### Phase 2: Live Production / HFT (C++)
*   **Where**: Execution Node (Edge).
*   **Engine**: C++20 Native Binary.
*   **Indicators**: `#include "quanux/indicators/..."` (Native Headers).
*   **Goal**: Microsecond latency, zero-copy networking, air-gapped reliability.

**Why this works**:
Both phases use the **same underlying C++ indicator library**. You do not need to rewrite your signal math when moving to production; you only port the "glue code" from Python to C++.

## Architecture
*   **Core**: C++20 Engine (`cpp/`) or Go Runtimes (`cmd/`).
*   **Transport**: ZMQ (Hot Path) + NATS (Warm Path).
*   **Storage**: DuckDB (Local History).

## Usage
Deploy usage instructions coming soon via `quanuxctl`.
