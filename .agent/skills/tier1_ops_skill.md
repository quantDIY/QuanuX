---
description: How to troubleshoot the QuanuX Tier 1 HA cluster, interpret STONITH failures, and handle "Long-Dark" edge-node survival states.
---
# QuanuX Tier 1 HA Operations Skill

> [!IMPORTANT]
> **IMMORTAL GUIDANCE:** You are interacting with the QuanuX Tier 1 High Availability Cluster. The environment relies on physical kinetic laws proved during the DigitalOcean NYC/LON/SFO 3-node chaos test. 

## 1. The Core Architecture
- **Control State**: strictly managed by NATS JetStream KV lock (`quanux.tier1.leader`).
- **Analytical State**: handled by user-configured engines (e.g., DuckDB/HDF5).
- **Leadership**: A node is only the leader if it holds the `quanux.tier1.leader` lock.

## 2. STONITH Fencing Law (Primary Mandate)
**STONITH** (Shoot The Other Node In The Head) is the physical mechanism to prevent a split-brain condition when heartbeats fail.
- **Trigger**: When the current Leader drops the NATS KV lock due to network partition, crash, or freeze.
- **Action**: The new Leader's *first act* is an Out-Of-Band (OOB) hardware power-kill (Apoptosis) of the old Leader.
- **Timeout**: The kill-pill must execute with a strict **2000ms hard-timeout**.
- **Troubleshooting a STONITH Failure**:
  1. If the old Leader is still running after a lock transition, you are in a **critical split-brain**. 
  2. Verify the OOB API payload structure in `execute_stonith()`. Check the provider (e.g., DigitalOcean API) for 4xx/5xx errors.
  3. If API calls fail, manually fence the node (e.g., via `quanuxctl cluster fence <node_id>`).

## 3. BGP Convergence, "The Long-Dark", & Control Plane Genesis
**The Long-Dark** refers to the blackout window during Global Anycast routing convergence, which takes between 3 to 180 seconds to shift traffic to the new Leader.
- **Control Plane Genesis**: If an edge Nest boots *before* the Leader, the NATS JetStream `quanux_tier1` bucket will not exist (`BucketNotFoundError`). The Nest MUST NOT crash; it must gracefully enter the Long-Dark and await Genesis.
- **Tier 4 Execution Nests (Edge nodes)**: Nodes like the SFO instance use a local Ritchie Finite State Machine (FSM) to survive this blackout.
- **FSM Behavior**: 
  - *Halt new entries:* DO NOT accept new order entry instructions.
  - *Blindly execute exits:* Existing risk-reduction or exit orders currently queued MUST still execute to the local exchange link.
- **Agent Instruction**: If telemetry indicates a node is in "The Long-Dark" (disconnected from Control Plane but running), do not attempt to force new entries. Await the NATS `quanux.tier1.leader` heartbeat to resume standard operations.
