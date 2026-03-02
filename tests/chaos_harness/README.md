# QuanuX Local Chaos Engine Harness

This directory contains an executable local test harness to verify the High Availability physical invariants of the QuanuX Tier 1 Control Plane without requiring a DigitalOcean cluster. It spins up a simulated 3-node topology using a local NATS server.

## Prerequisites
1. A running local NATS JetStream server.
   ```bash
   nats-server -js
   ```

## The Simulation Topology
- **Leader Node (`leader.py`)**: Represents the primary Tier 1 Control Plane node holding the KV lock.
- **Follower Node (`follower.py`)**: Represents the hot-standby Node waiting for a lock drop to execute STONITH.
- **Execution Nest (`nest.py`)**: Represents a Tier 4 edge node that will trigger the Ritchie FSM ("The Long-Dark") during a Control Plane partition.

## Execution Sequence (The Chaos Test)

1. **Start the Nest**
   ```bash
   # In terminal 1
   python tests/chaos_harness/nest.py
   ```
   *Expected:* The nest starts, drops into the "Long-Dark" FSM initially since there is no leader.

2. **Start the Leader**
   ```bash
   # In terminal 2
   python tests/chaos_harness/leader.py
   ```
   *Expected:* The Node claims the `quanux.tier1.leader` lock. The `nest.py` terminal detects the heartbeat and exits the Long-Dark, resuming entries.

3. **Start the Follower**
   ```bash
   # In terminal 3
   python tests/chaos_harness/follower.py
   ```
   *Expected:* The follower connects, sees the active lock, and enters Hot-Standby mode.

4. **Trigger Partition (Chaos Event)**
   Kill the `leader.py` process with `Ctrl+C` (or `kill -9` to simulate power loss).

5. **Observe the Physics**
   - The JetStream TTL expires (5 seconds).
   - **Follower Terminal:** Detects dropped lock, steals the crown, and fires the OOB STONITH API call against the fallen Leader ID. The Law of Verified Death is observed.
   - **Nest Terminal:** Detects missing hearbeat, BGP convergence lag, and enters "The Long-Dark," halting new entries and blindly managing exits until the Follower confirms leadership.
