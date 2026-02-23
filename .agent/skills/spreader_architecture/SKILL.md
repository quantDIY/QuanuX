---
name: QuanuX Spreader Architecture
description: Core architectural pattern and deployment pipeline for building QuanuX-Spreader strategies utilizing the 59ns Lock-Free Dual-Thread loop.
---

# QuanuX Spreader Architecture

> [!IMPORTANT]
> **IMMORTAL GUIDANCE FOR AGENTS:** When generating or modifying `QuanuX-Spreader` strategies, you must strictly adhere to the architecture described below. You must perfectly understand the separation of concerns between the **Execution Node** (a dumb networking stub), the **Spreader Engine** (the 59ns dual-thread C++ application), and the **Strategy** (the math formula).

## 1. The Separation of Concerns

### A. The Execution Node (The Remote Shell Stub)
The Execution Node is typically a bare-metal server co-located at an exchange. 
*   **What it does:** It manages direct market access (DMA) connections to broker servers and handles NATS/ZMQ cluster telemetry. 
*   **What it DOES NOT do:** It possesses **zero** trading logic. It does not even contain the 59ns execution loop. It sits live and awaits a fully compiled, weaponized trading application to be deployed onto it. 

### B. The QuanuX-Spreader Engine (The Application)
This is the ultra-high-performance C++ executable. It contains the 59-nanosecond, Lock-Free Dual-Thread architecture (described in Section 2). 

### C. The Strategy (The Math)
The mathematical formulas and specific instrument relationships (e.g., the spread ratio between CLZ4 and CLF5). These define *what* the Spreader Engine is calculating on its threads. If the strategy code includes the OnixS SDK and API keys, the engine natively utilizes the OnixS FIX implementation; otherwise, it falls back to QuickFIX (both using identical calling conventions).

## 2. The 59ns Dual-Thread, Single-Core Architecture
Inside the assembled `QuanuX-Spreader` application, operations occur on a single pinned CPU core utilizing two highly specialized threads communicating via an L1 cache-aligned `quanux::SPSCQueue`.

*   **Thread 1: The Innode Data Pipe (Producer)**
    *   Ingests raw binary market data from the Execution Node's DMA connection.
    *   Instantly formats the payload into a strictly cache-aligned 64-byte `quanux::MarketTick`.
    *   Executes the injected **Strategy** math directly on this aligned memory (e.g., calculating Locke differentials).
    *   If the math triggers, pushes an `Event` struct to the lock-free queue.
    *   Lazily flushes the payload to DuckDB (this is a sideshow, never blocking the math).

*   **Thread 2: Strategy Order Entry & FIX Layer (Consumer)**
    *   Spin-waits (`_mm_pause()`) on the queue in nanoseconds.
    *   Pulls the `Event`.
    *   Manages the FIX / OnixS protocol. It holds the direct connection to the broker. 
    *   **The Feedback Loop:** Because it executes on the same core, it retains direct reverse-access to the ongoing flow of Thread 1. When Leg A fills via FIX, Thread 2 can aggressively inject the Leg B offset directly against the incoming data stream in sub-microseconds.

## 3. The Deployment Pipeline
Agents generating spreader strategies must understand the sequence of pushing an idea to production.

1.  **Generation:** AI or analysts draft the complex multi-leg spread formulas and configurations, storing them in `server/spreader-strategies/`.
2.  **Assembly & Compilation (`quanuxctl spreader package`)**: The `quanuxctl` CLI injects the specific strategy formula into the `QuanuX-Spreader` engine codebase. The system compiles the combined components into a single, highly-optimized executable binary.
3.  **Deployment (`quanuxctl spreader deploy`)**: The compiled `QuanuX-Spreader` application is transmitted over the network and deployed directly onto the waiting, remote **Execution Node** stub. 
4.  **Live Execution**: The Execution Node binds its raw DMA pipes to the newly deployed application, and the 59ns loop begins evaluating live tick data.

## 4. The 59ns Happy Path: Patterns & Constraints
To execute at 59 nanoseconds, strategy generation and code generation must adhere to strict hardware-level constraints. The engine enforces a "Zero-Drift Policy."

### A. The Zero-Jitter Memory Arena
The engine fundamentally treats the OS heap as non-existent during live execution. 
*   **No Allocations:** Agents must **never** instantiate `std::make_shared` or call `new` in the Thread 1 or Thread 2 hot paths.
*   **The Pool:** The engine pre-allocates cache-aligned `quanux::MemoryPool` arrays (fixed-size, power-of-two blocks) during the `init()` phase. Ticks and events are fetched using bitwise AND pointer math `& (Capacity - 1)` to eliminate modulo calculation latency and entirely avoid OS page faults.

### B. O(1) LOCF Price Matrices
Standard dynamic maps or string-based hashes (`std::map<string, double>`) are absolutely banned in the hot path. 
*   **Direct Indexing:** To execute multi-leg Last Observation Carried Forward (LOCF) spread calculations, the engine uses a cache-aligned `PriceMatrix`. Strategies must look up leg prices via direct array indexing using a `uint32_t instrument_id`. This executes in exactly 1 CPU cycle.
*   **Dirty Bits:** Leg updates are flagged using atomic sequence counters (`std::atomic<uint32_t> update_seq`) initialized with `memory_order_release`. This allows Thread 2 (FIX Order Entry) to instantly verify state changes via the core's L1 cache without ever locking or traversing the entire queue history.

### C. SHM/DMA Handshake Constraints
To prevent subtle memory corruption or L1 cache line eviction during the DMA handshake between Thread 1 (Math) and Thread 2 (Order Execution), the `StrategyState` payload is hard-capped.
*   **The 64-Byte Guard:** The `engine.hpp` explicitly enforces a structural size limit: `static_assert(sizeof(StrategyState) <= 64)`.
*   **Refusal to Forge:** If an AI agent attempts to design an overly bloated strategy state that exceeds this single cache line size, the CMake `quanuxctl spreader package` command will instantly fail the build. Keep your strategy matrix math tightly bundled!

### D. Compiler-as-a-Service (`CMakePresets.json`)
The QuanuX compilation pipeline leverages `CMakePresets.json` to turn the build environment into a deterministic API. Any fully packaged spreader binary is automatically run through the `spreader` configuration preset, guaranteeing it builds with `-march=native`, `-O3`, and Link-Time Optimization (`-flto`). This instructs the compiler to aggressively fuse the agent-generated strategy math directly alongside the 59ns loop logic.

## 5. The Cython Forge & Cryptographic Deployment
Strategy injection uses Cython rather than naive string templating (like Jinja2).
*   **Strict C-Types:** The Foundry parses the JSON IR and transpiles it into `strategy_injected.hpp` wrapped in `extern "C++"`. This guarantees the `StrategyState` structure uses strict C-types (`double`, `uint32_t`) and avoids 64-byte L1 cache spillage.
*   **Crucible Equality:** It simultaneously generates a `strategy_wrapper.pyx` file. This allows the exact same C++ `calculate()` block to be invoked natively by Python during Crucible backtesting, ensuring 100% mathematical parity between the simulation and live deployment.
*   **Forge Signatures (`.sig`):** When `quanuxctl spreader package` runs, it cryptographicly hashes the final machine-code binary (e.g., `quanux-spreader-063e07ea`) and outputs a `.sig` manifest.
*   **Deployment Gate:** The `quanuxctl spreader deploy` command reads the binary hash and validates it against the `.sig` manifest. If an agent or human attempts to alter the executable after packaging, the deployment will fail with `CRITICAL SHM ERROR: Binary checksum mismatch.`

## 6. Trial & Error Insights (Cautionary Tales)
When generating C++ for the 59ns Spreader, agents must be aware of these strict compilation traps that were uncovered during development:
1.  **The `-Werror` Unused Variable Trap:** C++ compilation is extremely strict. Handlers (like `main(int argc, char** argv)` or `init(StrategyState& state)`) that don't utilize their parameters will fail the `-Werror` optimization build. *Fix: Always cast unused variables to void: `(void)argc;`*.
2.  **The C vs C++ `typedef` Collision:** When importing C-libraries like `nats.h` into modern C++ headers, do not use naive forward declarations like `struct natsConnection;`. The C library declares `typedef struct __natsConnection natsConnection;`. *Fix: Use identical explicit typedef structures to avoid compiler `redefinition with different types` errors.*
3.  **CMake Build Directory Pathing:** Because the QuanuX root `CMakeLists.txt` delegates to `add_subdirectory(QuanuX-Spreader/cpp)`, the resulting `quanux_spreader` binary is placed deep inside the mirrored build directory (`build/QuanuX-Spreader/cpp/quanux_spreader`), NOT at the root of `build/`. Agents scripting CLI automation must accurately resolve this nested build target path.
