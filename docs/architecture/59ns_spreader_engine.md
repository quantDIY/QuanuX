# QuanuX Spreader: The 59ns Execution Engine Architecture

This document details the architecture, memory handling, and build philosophy behind the `QuanuX-Spreader` module. It is designed to act as a historical and technical reference for the "59ns Happy Path."

## 1. The Separation of Concerns
The QuanuX architecture strictly separates the routing topology from the mathematical execution:

*   **The Execution Node:** A dumb C++ networking stub deployed on bare-metal servers. It connects to the DMA pipes (NATS/ZMQ) and awaits strategy deployment.
*   **The Spreader Engine:** The core `QuanuX-Spreader` C++ application, containing the 59ns dual-thread loop.
*   **The Strategy:** The specific mathematical formula (e.g., CLZ4/CLF5 Locke Differentials). This is forged directly into the Engine via the Cython transcompiler pipeline.

## 2. The 59ns Dual-Thread, Single-Core Architecture
To eliminate context switching and scheduler jitter, the engine pins exactly two threads to a single physical CPU core.

### Thread 1: The Innode Data Pipe (Producer)
This thread listens directly on the NATS inner dispatch socket.
*   **NATS Zero-Copy:** `on_nats_message` performs a `reinterpret_cast` directly on the incoming byte buffer.
*   **The MemoryPool Pivot:** Data is instantly `memcpy`'d (via SIMD optimization) into the `quanux::MemoryPool` to ensure absolute pointer stability.
*   **The One-Pass Update:** The Thread updates the LOCF Price Matrix and invokes `Strategy::calculate()`. If the math triggers, it pushes an event to an `SPSCQueue`.

### Thread 2: Strategy Order Entry (Consumer)
This thread handles the FIX or OnixS order entry.
*   **Nano-Spinning:** It uses hardware `_mm_pause()` instructions to spin-wait on the lock-free queue without yielding the CPU to the OS.
*   **The Pre-Flight Check:** Right before firing a Leg B order, it directly queries the `PriceMatrix` for the most recent offset to avoid "Stale-Quote Risk."

## 3. Memory Constraints: The Zero-Drift Policy
To prevent L1 cache invalidation, the system enforces strict memory rules:
1.  **The Zero-Jitter Arena:** No `new` or `std::shared_ptr` calls are allowed in the hot-path. Everything is pre-allocated via `MemoryPool` instances that use `& (Capacity - 1)` bitwise indexing.
2.  **O(1) LOCF Price Matrices:** `std::map` lookups are banned. Leg prices are stored in a fixed `std::array<PriceEntry, 8192>` and accessed via pointer arrays indexed by `instrument_id`.
3.  **Dirty Bits:** State updates are flagged using `std::atomic<uint32_t> update_seq` with `memory_order_release`. Thread 2 reads these with `memory_order_acquire`, allowing lockless observability across the core.
4.  **The 64-Byte Handshake:** A static assertion prevents the `StrategyState` payload from exceeding 64 bytes, preventing cache-line spillover during the Thread 1 to Thread 2 DMA handshake.

## 4. The Cython Forge Pipeline
Instead of loading strategy scripts dynamically, QuanuX statically compiles them into the 59ns engine.

1.  **IR Parsing:** `quanuxctl spreader package` reads a Strategy JSON IR (Intermediate Representation).
2.  **Cython Transpilation:** It transpiles the logic into an `extern "C++"` header (`strategy_injected.hpp`) and a Python Wrapper (`strategy_wrapper.pyx`). The wrapper allows the exact same C++ logic to drive the Python Crucible Backtesting engine.
3.  **Compiler-as-a-Service:** It invokes `CMakePresets.json` to compile the binary with `-march=native`, `-O3`, and `-flto`.

## 5. Security: Cryptographic Deployment Gate
Because strategies are injected directly into C++ and compiled to machine code, a strict security mechanism prevents rogue script deployments.
*   During the `package` phase, the final binary is SHA-256 hashed.
*   A `.sig` manifest file is generated (e.g., `quanux-spreader-[HASH].sig`).
*   During the `deploy` phase, `quanuxctl` recalculates the target binary's hash and cross-references the manifest. Mismatches immediately halt deployment.

## 6. Trial & Error Insights
During development, the engineering team overcame several strict C++ compilation challenges:
*   **The `-Werror` Unused Variable Trap:** C++ functions (like the NATS callback or Strategy initialization) must cast unused variables like `argc` or `sub` to `(void)` to bypass strict `-Werror` build failures.
*   **The C vs C++ `typedef` Collision:** Implementing C headers (like NATS) inside modern C++ files requires precise `typedef struct __name` declarations rather than naive `struct` forward declarations.
*   **CMake Mirrored Target Paths:** The `CMakePresets.json` using `add_subdirectory` places final executables inside nested build folders (e.g., `build/QuanuX-Spreader/cpp/quanux_spreader`), requiring precise path resolution in the python CLI scripts.
