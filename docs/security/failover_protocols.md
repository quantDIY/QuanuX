# QuanuX Security & Failover Protocols

## 1. Overview
The security of the QuanuX Tier 1 Global Supercluster relies equally on cryptography and brutal pragmatism. When a failover event is triggered, the cluster cannot assume the old Leader is "dead" simply because it vanished from the network quorum; it must be **proven** dead. This document formalizes the STONITH (Shoot The Other Node In The Head) protocol, also known as the Apoptosis Directive.

## 2. The STONITH Protocol (Apoptosis)
A "Split-Brain" scenario—where two Tier 1 Orchestrators believe they are the Leader and stream conflicting commands to Execution Nests—is an unrecoverable, catastrophic event.

### 2.1 The Prime Directive of Promotion
When a Follower successfully acquires the `quanux.tier1.leader` NATS JetStream KV lock via Raft consensus, **it is strictly prohibited from emitting orchestration commands** until it has fully terminated the previous Leader.

### 2.2 Fencing Mechanisms
The new Leader will execute a `STATE_HALT` kill-pill against the fallen Leader's `Node_ID`. The stark reality of distributed systems is that **Network Partitions** render software fencing useless. If the old Leader is unreachable because its primary switch failed, it is still alive locally. Sending a cryptographic token or trying to SSH into it will fail. Thus, QuanuX prioritizes physical infrastructure segregation:

1. **Hardware Fencing (IPMI / iLO / PDU) via Out-Of-Band (OOB) Network:**
   **[PRIMARY MANDATE]** Hardware physical power fencing is the ONLY valid STONITH in a network split. The new Leader sends a kill-pill over a physically separate OOB ethernet network wired to a distinct management switch. It connects directly to the Baseboard Management Controller (BMC) or the Power Distribution Rack to physically kill power to the old Leader.
2. **OS-Level Fencing (SSH / API Kill):**
   *(Secondary)* If the OOB network is unavailable but the OS is reachable, the new Leader issues a kernel-level `sysrq-trigger` or tears down the supervisor.
3. **Cryptographic Fencing (The Ritchie Protocol Extension):** 
   *(Tertiary/Fallback)* The new Leader broadcasts a signed global banishment token across JetStream. NATS will brutally reject any further connections originating from the old Leader's TLS identity, acting as a final logical fence if power cannot be cut.

*If all fencing attempts fail or block, the new Leader must ABORT its lock acquisition via a hard-timeout to prevent the "Infinite Blocking Trap" and log a `CRITICAL_PENDING` alarm to the Architect.*

## 3. The Millisecond Timeline of Failover Execution
The life-cycle of a high-availability failover across continents happens far faster than a human operator can react—up until the grim reality of global BGP propagation.

*   `T+0ms`: The active Tier 1 Leader (e.g., Aurora) physically crashes or drops its network connection to the Quorum.
*   `T+Nms`: The `quanux.tier1.leader` JetStream lock's TTL (Time-To-Live) heartbeat expires.
*   `T+N+10ms`: Raft election begins. Followers in Carteret and Frankfurt detect the missing leader.
*   `T+N+25ms`: The Frankfurt node, having a faster consensus ping with the remaining Quorum, acquires the `quanux.tier1.leader` lock.
*   `T+N+30ms`: **Apoptosis Initiated.** Frankfurt fires a hardware kill-pill (STONITH) to Aurora's physical IPMI over the OOB network.
*   `T+N+45ms`: **Event Sourcing Replay.** Frankfurt replays the JetStream log in deterministic order to rebuild its risk and orchestration state, adopting the precise memory footprint of Aurora right before the crash.
*   `T+N+50ms`: **BGP Route Advertisement.** The VIP routing shifts. Frankfurt broadcasts it is now the origin of the Control Plane IP via BGP Anycast (or GARP for intra-DC local failovers).
*   `T+N+55ms`: **The Long-Dark Blackout Begins.** While GARP works instantly locally, BGP convergence across the global internet takes anywhere from 3 to 180 seconds. The Edge Nests enter the "Long-Dark".
*   `T+2000ms - T+120s`: **Edge Nest Survival (The Ritchie FSM).** The Tier 4 Fiber Nests know the Control Plane is dark. They throttle or halt new strategy entries locally, blindly executing their existing exit logic and risk liquidation checks via raw sockets until global routing converges.
*   `T+Route_Converged`: **Reconnection.** The global physical network finally routes the VIP to Frankfurt. Nests seamlessly reconnect. Standard Operation Resumes.

## 4. Unattended Operations
Tier 3 and Tier 4 Nests are biologically resilient. If the entire Control Plane is briefly severed, Nests will freeze non-essential risk updates but **will continue deterministic order execution and local liquidation checks** via raw sockets. They are designed never to panic in the dark.
