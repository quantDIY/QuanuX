# HA Implementation Roadmap & CLI Expansion Plan

This document serves as the exact engineering blueprint for migrating the QuanuX Tier 1 Server from a single-node Python orchestrator into a fully distributed Global Supercluster.

## Phase 1: NATS JetStream vs Analytical Boundary
**Objective:** Replace local state management with a globally distributed KV lock while enforcing the boundary between Execution State and Analytical Memory.

1. **Initialize Global Bucket:** Create a NATS JetStream Key-Value bucket named `quanux_cluster_state` replicated across $N$ geographic regions.
2. **Lock Definition:** Define the primary lock `quanux.tier1.leader`.
3. **State Reflection (The Dichotomy):** 
   - **Control State (NATS JetStream):** Risk profiles, Supervisor limits, active deployments, and real-time orchestration events are strictly appended to the JetStream Event Log for immediate HA replay.
   - **Analytical State (Hybrid):** Historic tick data, heavy backtesting memory, and massive order-flow archives must NEVER be stored in NATS. They are safely offloaded to a user-configured storage engine (DuckDB, HDF5, NAS). HA Failover strictly guarantees the Control State, not the analytics volume.

## Phase 2: Python Background Leader Election Loop
**Objective:** Implement the Active-Passive heartbeat and Raft observer within the FastAPI backend with defensive STONITH timeouts.

1. **The Observer Task (FastAPI Lifespan):** The Tier 1 Server is driven by FastAPI. The background `asyncio` task (`GlobalSentinelLoop`) does not exist in a vacuum; it is spun up and managed by the FastAPI lifespan context manager (or startup/shutdown events). Furthermore, the FastAPI routing logic must be Raft-aware: if the node is a Follower, specific orchestration endpoints must automatically reject or redirect traffic until the node is promoted to Leader.
2. **Heartbeat Maintenance:** If the server is Leader, it writes the current timestamp to `quanux.tier1.leader` every `50ms`. 
3. **The Watcher:** If the server is a Follower, it establishes a JetStream Watcher on the lock. If the lock's TTL is exceeded (e.g., no update for `250ms`), the server attempts a targeted `Update` with its own `Node_ID` to seize the lock.
4. **Apoptosis Hook (Defended):** Upon acquiring the lock, the backend triggers `execute_stonith(old_leader_id)`. **CRITICAL:** This call must have a strict hard-timeout (e.g., `2000ms`). If the IPMI interface of the dead datacenter is offline, it cannot block infinitely. If the script hits the timeout, it abandons the lock, enters a `CRITICAL_PENDING` state, and fires a severe alarm via `quanuxctl`/SMS/PagerDuty to the Architect.
5. **State Rehydration:** Once Fencing is verified, the server replays the NATS Event Log to rehydrate the application state and begins accepting `quanuxctl` and Nest connections.

## Phase 3: The `quanuxctl` CLI Expansion
**Objective:** Grant the Architect "God-Mode" over the Raft cluster and manual failover hierarchy. Any automated clustering protocol must have deterministic manual overrides.

The `quanuxctl` CLI will be expanded to include the `cluster` command group.

### `quanuxctl cluster status`
*   **Action:** Queries NATS for the telemetry of the global supercluster.
*   **Output:** 
    *   Identifies the current **Leader** (Node ID, Region, Uptime).
    *   Lists all **Followers** (Node IDs, Ping to Leader, Replay Lag).
    *   Displays the health and state of the `quanux.tier1.leader` lock.

### `quanuxctl cluster promote <node_id>`
*   **Action:** Forces a manual Raft election override.
*   **Execution:** Administratively commands the current Leader to drop the lock and artificially boosts the priority/election-timer of the specified `<node_id>` so it is guaranteed to become the new Leader.
*   **Use Case:** Pre-emptive maintenance of a datacenter or shifting latency footprints before major economic releases.

### `quanuxctl cluster demote`
*   **Action:** Forces the current Leader to step down gracefully without explicitly assigning a successor.
*   **Execution:** The Leader deletes its lock on `quanux.tier1.leader` and enters a 5-second backoff period where it refuses to vote OR run for election, allowing the remaining Followers to elect a new Leader.

### `quanuxctl cluster fence <node_id>`
*   **Action:** Manually triggers STONITH (Apoptosis) against a rogue or "zombie" node.
*   **Execution:** Bypasses Raft consensus entirely and immediately fires the deepest available Fencing mechanism (Cryptographic -> OS -> Hardware) against a specific Node ID. 
*   **Use Case:** Resolving complex network splits or permanently blinding a node that has been compromised or is behaving erratically outside of normal cluster logic.

---
**Execution Mandate:** Development must proceed linearly from Phase 1 to Phase 3. The foundational AI context (tier1_ha_skill.md) provides the parameters. Code generation algorithms are to strictly reference this plan when structuring the `quanuxctl` Typer framework extensions.
