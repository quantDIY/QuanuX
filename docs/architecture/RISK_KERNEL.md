# QuanuX Risk Kernel Architecture: The Hydration Gate

This document serves as the master physics manual for Prime Broker Risk Officers and Institutional Auditors analyzing the QuanuX execution plane. It guarantees that the system is mathematically incapable of violating capital limits or order size constraints, regardless of network partitions, hardware failure, or dynamic "split-brain" cluster topologies.

## The Core Concept: Sovereign Isolation vs. Global Consensus
In high-frequency trading (HFT), risk checks must be performed in sub-nanosecond timeframes natively within the L1/L2 CPU Cache. The system cannot afford to query a database, perform a REST API call, or map shared memory across a network switch to authorize a trade.

To achieve this while maintaining global risk compliance across a cluster of nodes, QuanuX relies on the **Hydration Gate Doctrine**.

### 1. Optimistic Local Execution (The Hot Path)
During active trading, the QuanuX C++ Sovereign Engine is strictly isolated from the global network regarding risk logic.
- **Cycle Cost**: ~1 CPU Cycle.
- **Mechanism**: The execution loops (`quanux-spreader(1)`) check only local RAM structures (specifically, an inline `check_trade()` matrix updated via deterministic logic).
- **Rule**: The C++ Engine *NEVER* listens to the global network (CNATS Mesh) synchronously to validate a trade. It operates entirely on localized, optimistic confidence in its own internal counters. 

### 2. Pessimistic Global Hydration (The Cold Gate)
The moment an edge node crashes, network partitions occur, or a leader election fails, the execution plane acts purely pessimistically. 

If Node A (e.g., SFO) fails, and Node B (e.g., NYC) successfully executes STONITH (Shoot The Other Node In The Head) to seize the Leadership Lock, Node B is **mathematically barred** from opening its TCP socket to the matching engine.

Before Node B is legally allowed to execute a single bit of trade data:
1. **The Hydration Barrier**: It must invoke a synchronous, blocking read from the strictly consistent CNATS JetStream KV store.
2. **State Pull**: It pulls the absolute last globally confirmed risk state (e.g., "SFO had executed 402/500 contracts before failure") dumped by the previous leader.
3. **Seeding the Matrices**: The local memory matrices inside the C++ Engine are strictly overridden by this global state.
4. **The `is_hot` Boolean**: Only after the memory matrices pass a SHA-256 verification of the KV state does the C++ Engine flip `bool is_hot = true;`. 

Until `is_hot == true`, the `check_trade()` inline function universally returns `false`.

### 3. Hard vs. Soft Limit Bifurcation
To ensure institutional safety, risk parameters are structurally bifurcated:

**Hard Limits (C++ Binary Constants):**
Constraints like `MAX_ORDER_QTY` (e.g., "Never send an order larger than 50 contracts in one packet") or `MAX_PRICE_DRIFT_TICKS` are compiled directly into the C++ `constexpr` definitions. They cannot be changed without stopping the engine, rewriting the C++ source, passing CI pipelines, and deploying a newly signed binary. They are immune to configuration errors.

**Soft Limits (Hydrated Matrix variables):**
Constraints like `GLOBAL_NOTIONAL_CAP_USD` (e.g., "$10,000,000 intraday exposure") are mapped into the C++ local RAM. These are controlled and distributed by the Hydration Gate natively upon boot, or sequentially and asynchronously updated by authorized `quanuxctl` commands hitting the CNATS mesh during the trading day.

## Summary for Auditors
QuanuX's Risk Kernel relies on **local physics bounded by global initialization.** The engine checks local memory for extreme, sub-microsecond speed, but that local memory is violently gated upon boot or failover scenarios by a perfectly consistent global state lock. It is structurally impossible for an orphaned node to accrue risk without hydrating the global consensus first.
