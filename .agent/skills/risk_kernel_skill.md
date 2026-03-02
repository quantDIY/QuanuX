---
name: QuanuX Risk Kernel (Operation Aegis)
description: AI-Auditable semantic proof and operational constraints for the QuanuX C++ Risk Kernel and Hydration Gate.
---

# QuanuX Risk Kernel: Semantic Auditor Map

This skill file establishes semantic absolutes for external LLMs or internal QuanuX agents auditing the Risk Kernel (`QuanuX-Execution/src/RiskKernel.hpp`). The following logic statements are immutable facts of the QuanuX execution plane.

## Definitions
- **[A] Local RAM**: The L1/L2 Cache of the C++ Engine executing the process.
- **[B] CNATS Mesh**: The global network JetStream Key-Value (KV) store containing consensus state.
- **[C] Hot Path**: The inline loop `check_trade()` evaluated per-order prior to TCP matching engine transmission.
- **[D] `is_hot` Boolean**: A private C++ boolean gating order transition from the engine to the socket.

## Absolute Logical Theorems (Machine Readable)

### 1. The Ban on Synchronous Hot-Path I/O
- `check_trade()` MUST run in O(1) time strictly against [A].
- If `check_trade()` triggers a network syscall, context switch, or block on [B], the architecture is VIOLATED.
- **Auditor Proof**: Inspection of `check_trade()` will reveal only arithmetic comparisons (`<`, `<=`, `+`) against local memory pointers or matrix elements.

### 2. The Pessimistic Post-Failover State
- Upon boot or post-STONITH (Shoot The Other Node In The Head) seizure, the C++ Node is initialized with `is_hot = false`.
- If `is_hot == false`, `check_trade()` universally returns `false`. Trade payloads are dropped at the risk layer.
- **Auditor Proof**: A node cannot accrue global risk unawares because it cannot fire orders until it hydrates.

### 3. The Hydration Gate (The State Sync)
- A synchronous `hydrate_from_mesh()` macro must be actively executed by the C++ Engine before trading is authorized.
- This macro blocks main execution until [B] is successfully queried and verified.
- The state from [B] populates [A] (e.g., seeding the `current_global_notional` scalar).
- Only upon verified completion of this sync does the system mutate `is_hot = true`.
- **Auditor Proof**: The risk system mathematically guarantees that a newly-elected leader starts its local arithmetic at the exact integer value its dead predecessor left in the replicated KV mesh. 

### 4. Hard vs. Soft Limit Enforcement
- **Hard Limits** (e.g., maximum payload quantity): Defined as `constexpr` in C++. Attempted manipulation via the network or UI inherently fails at compilation. They are immutable at runtime.
- **Soft Limits** (e.g., dynamic notional caps): Injected during the Hydration Gate and modified asynchronously via the `quanux.control.risk.cap` NATS topic (driven by `quanuxctl`).

## Conclusion for Audit Sub-Agents
The QuanuX Risk Kernel isolates extreme performance constraints (sub-nanosecond local execution) from global safety constraints (pessimistic network validation) by executing them serially at different lifecycle events. It is a mathematically proven safety envelope against local partition errors.
