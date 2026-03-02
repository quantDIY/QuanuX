# QuanuX Tier 1 HA: The 3:00 AM Panic Runbook

> [!CAUTION]
> **READ THIS FIRST:** If you are reading this at 3:00 AM, the cluster is screaming. Do not think. Execute the physical laws of the system.

## The Rule of the KV Lock
**The Truth lives ONLY in NATS JetStream.** Whoever holds the `quanux.tier1.leader` KV lock is the Leader. No exceptions.

---

## 1. If leader dies, do this
**Symptoms:** The Leader node becomes unresponsive or drops off the network.
**Standard Protocol:** The system is designed to auto-failover and STONITH the dead node. If you receive an alert that the leader died but the auto-failover succeeded, **no immediate action is required**. Monitor the cluster until the dead node can be physically replaced.

---

## 2. If OOB network unavailable, do this (Split-Brain / Failed STONITH)
**Symptoms:** You have two nodes claiming to be Leader. The API is flapping.
**Cause:** The OOB hardware power-kill (STONITH) failed its 2000ms timeout during an election.

### Recovery Steps:
1. **Identify the true holding node:**
   ```bash
   quanuxctl cluster status
   ```
2. **Manually Fence the Usurper:**
   Identify the node that DOES NOT hold the lock and terminate it with extreme prejudice.
   ```bash
   quanuxctl cluster fence <rogue_node_id>
   ```
3. **Verify App State:** Ensure FastAPI on the new Leader is propagating the heartbeat loop.

---

## 3. If KV stuck, do this (Total Cluster Freeze)
**Symptoms:** The Leader is dead (e.g., kernel panic, completely dark), but no Follower is spinning up to take its place.
**Cause:** NATS JetStream edge-case where the Leader disconnected dirty, but the lock TTL hasn't expired or is hung.

### Recovery Steps:
1. **Force Promotion on a Follower:**
   Pick the healthiest Follower (e.g., the closest geographic standby) and force Raft election override.
   ```bash
   quanuxctl cluster promote <fallback_node_id>
   ```
2. **If that fails, Demote the Ghost Leader:**
   ```bash
   quanuxctl cluster demote
   ```
3. Wait 3 seconds for the BGP and Anycast IP shift.

---

## 4. If edge execution nodes detach, do this (The "Long-Dark" & Control Plane Genesis)
**Symptoms:** Sub-nodes (Tier 4 Execution Nests like SFO) are dropping connection to the Control Plane but still executing trades, or they boot and print "Awaiting Control Plane Genesis".
**Cause:** 
- *The Long-Dark:* Global Anycast routing takes 3 to 180 seconds to shift BGP convergence.
- *Genesis Race Condition:* A Nest booted before the Leader and encountered a `BucketNotFoundError` because the NATS bucket doesn't exist yet.
**Action:** Let the Ritchie FSM run. *Do nothing.* Edge nodes will blindly execute exits and halt entries. They will safely wait in the dark and automatically reconnect when NATS becomes reachable or the Leader creates the bucket.

---

## Reference Walkthrough: DigitalOcean 3-Node Chaos Engineering Test
*Completed March 2, 2026 across NYC, LON, SFO components.*

This deployment validates the physical boundaries of our high-availability architecture.
1. **The Setup**: 
   - NYC (Node A): Primary Leader holding NATS KV lock.
   - LON (Node B): Follower, watching NATS.
   - SFO (Node C): Tier 4 Execution edge node.
2. **The Induction**: NYC eth0 interface was artificially dropped (simulating catastrophic instance failure).
3. **The Lock Release**: NATS JetStream eventually registered the NYC session dropped. The lock was released.
4. **The STONITH execution**: LON acquired the lock. LON's Sentinel loop immediately triggered a DigitalOcean API execution to power-off NYC within 2000ms to prevent split-brain if eth0 returned.
5. **The Long-Dark**: SFO lost connection to NYC. SFO engaged the Ritchie FSM, blocking new entries but dumping active exposure. 
6. **Convergence**: Within ~74 seconds, Global Anycast BGP converged to LON. SFO reconnected to LON, recognized the new Leader heartbeat, and resumed normal operation.
