---
description: The Physical Laws of Global High Availability (HA) for QuanuX Tier 1 Leader Nodes and the Active-Passive NATS Supercluster.
---

# QuanuX Tier 1 High Availability (HA) Skill File

> [!IMPORTANT]
> **IMMORTAL GUIDANCE:** This document establishes the absolute physical and cryptographic laws of the QuanuX Tier 1 Global Supercluster. Any code, architecture, or CLI tools you write concerning Tier 1 routing, orchestration, or node control MUST strictly adhere to this protocol.

## 1. The NATS JetStream KV Lock (Leader Election)
The Tier 1 Leader is determined *strictly* by who holds the `quanux.tier1.leader` key via Raft consensus. 
- There is no ambiguous state. A node is either the Leader or a Follower.
- Holding the KV lock grants the Tier 1 server total god-mode over the Global Sentinel network.
- When the Leader's TTL (Time-To-Live) on the lock expires without a heartbeat, the Raft consensus automatically allows a hot-standby Follower to acquire the lock and promote itself.

## 2. Active-Passive Global Nodes
Only **ONE** Tier 1 Server acts as the active orchestrator.
- **The Leader:** Actively commands the Nests, deploys risk updates, and emits orchestration directives.
- **The Followers:** Maintain absolute silence. They are hot-standby replicas observing the NATS JetStream state, passively syncing to the exact nanosecond of the Leader's event log without taking any authoritative action.

## 3. The STONITH Fencing Law (Preventing Split-Brain)
This is the most critical axiom of QuanuX high availability. If a Follower promotes itself to Leader, its *very first cryptographic act* MUST be to execute a precise Fencing operation.
- **The OOB Network Mandate:** Fencing must be executed via IPMI/PDU over a strictly separate Out-Of-Band (OOB) Ethernet network. Network partitions mean cryptographic tokens or SSH will fail; hard-power cycling is the only absolute proof of death.
- **No Infinite Blocking:** The STONITH sequence MUST have a severe hard-timeout (e.g., 2000ms). If it fails to reach the BMC/PDU, it must abort and transition to `CRITICAL_PENDING` with alarms sent to the Architect. It cannot block the event loop infinitely.
- We cannot permit a Split-Brain reality where two Tier 1 nodes believe they are the Leader and issue conflicting logic to the Tier 4 Fiber Nests. 

## 4. BGP Convergence & The "Long-Dark"
- Do not assume immediate route convergence. Global BGP shifts take 3 seconds to 3 minutes.
- Tier 4 Nests must be programmed to survive the "Long-Dark," halting *new* entries and executing *existing* exit logic blindly until routes converge.

## 5. State Segregation (NATS vs. Analytical Storage)
- **NATS JetStream** holds ONLY the Control State (active deployments, risk updates).
- **Analytical Storage (Hybrid Choice)** holds historic/analytic Memory (tick data, deep backtest results). The end-user configures whether this is DuckDB, HDF5, or another NAS. HA failover only guarantees NATS orchestration.

## 6. The CLI Authority (`quanuxctl`)
Automated Raft consensus governs normal operation, but the Architect commands the system via `quanuxctl`.
- `quanuxctl` is the ONLY manual interface authorized to override the Raft election.
- Use `quanuxctl` to force failovers, demote a struggling Leader, or permanently fence a rogue node from the cluster.
- When generating strategy deployment or node orchestration logic, remember that `quanuxctl` can seamlessly interrupt or redirect the system flow through these administrative paths.

**When analyzing or extending QuanuX HA features, you must always verify that your solution complies with OOB STONITH limits, BGP delay realities, Analytical State Segregation, and the singular authority of the CLI.**
