# QuanuX Global High Availability Architecture

This document dictates the design and structural reality of the QuanuX Active-Passive NATS Supercluster—a globally distributed Control Plane orchestrating decentralized Tier 4 Nests in multiple financial hubs (e.g., Aurora, Carteret, Frankfurt).

## 1. The Global Sentinel Vision
QuanuX operates under the paradigm that the Data Plane is ruthless and hyper-localized (C++ execution loops over bare-metal Linux sockets), while the Control Plane is globally distributed, highly available, and impervious to catastrophic localized failure. 

To achieve this, QuanuX leverages an **Active-Passive Global Tier 1** mechanism backed by a NATS JetStream Supercluster. 

### The Split
*   **Data Plane (Tick-to-Trade):** 59ns lock-free Dual-Thread execution in Tier 4 Fiber Nests. This layer operates out-of-band of the central node.
*   **Control Plane (Orchestration & Risk):** Synchronous, deterministic, Raft-driven governance managed by the Tier 1 Global Leader Server.

## 2. Infrastructure Anatomy
The Tier 1 Control Plane is structured via **Leader Election**:
*   **Tier 1 Leader:** The sole commander holding the JetStream KV lock (`quanux.tier1.leader`). Responsible for emitting immutable orchestration logs, adjusting risk metrics, and managing the Biological Lore (e.g., triggering Apoptosis).
*   **Regional Followers:** Live in datacenters worldwide. They are silent hot-standbys that persist the JetStream event log.

---

## 3. Failover Sequence: The Millisecond Anatomy of a Crash

When a Tier 1 Leader experiences physical destruction, network segmentation, or fatal OS panic, the QuanuX cluster executes a mathematically deterministic failover protocol.

### Step 1: Leader TTL Expiration
The Tier 1 Leader sends a high-frequency heartbeat holding the JetStream lock. If the cluster goes `N` milliseconds without a heartbeat, the lock's TTL expires.

### Step 2: Follower Promotion
Raft consensus awakens the Followers. The fastest Follower (usually the geographically closest with the best ping to the quorum) instantly seizes the `quanux.tier1.leader` KV lock. At this point, it is logically the Leader.

### Step 3: STONITH Apoptosis (Fencing)
Before issuing a single command to the edge nodes, the new Leader issues a mathematically guaranteed kill-pill—a `STATE_HALT` command (Apoptosis)—directed at the physical ID of the fallen Leader. 
*   *Why?* It prevents a "Split-Brain." If the old Leader was severed by a **Network Partition**, it cannot be reached via normal SSH or software protocols. Fencing MUST occur over a strictly separate **Out-Of-Band (OOB) Hardware Management Network** (IPMI/iLO/PDU) to physically cut its power. Otherwise, the old Leader will resurrect locally when the partition heals, creating chaotic dual-command horizons.

### Step 4: Event Sourcing & Deterministic Replay
The new Leader replays the last uncommitted NATS JetStream log. By traversing the deterministic event sequence of the entire cluster, the new Leader rebuilds the exact working memory and risk state that the old Leader possessed micro-seconds before crashing. No configurations or deployments are dropped.

### Step 5: Global Anycast IP & The "Long-Dark" Reconnection
*   **Control Plane Routing:** Tier 1 IPs are configured as Virtual IPs (VIPs) using BGP Anycast. When the failover occurs, the new Leader triggers a BGP route update to shift traffic globally. 
*   **The "Long-Dark" Survival Mode:** The execution edge Nests detect a ping timeout. BGP route convergence across the global internet requires anywhere from 3 seconds to 3 minutes. The Nests do **NOT** panic, but they understand the reality of the propagation delay.
*   **Ritchie FSM (Finite State Machine):** During the blackout, Nests throttle or completely halt *new* strategy entries locally. They rely on their FSM to blindly execute *existing* exit logic via raw sockets. When the BGP routes finally converge, the physical internet seamlessly routes them to the new Leader node. Connection restored. State synchronized.

## 4. Summary
The QuanuX High Availability Architecture bridges biological resilience with institutional-grade networking. By coupling Raft election to STONITH fencing and separating the Control Plane from the localized Execution Plane, the cluster can dynamically survive the loss of master operational nodes worldwide.
