---
name: End-to-End Deployment
description: The mandatory workflow for deploying payloads to QuanuX Edge Nodes. Defines the True Sovereign Lifecycle.
---
# End-to-End Deployment

## Context & Rules
- Deployment to QuanuX Edge nodes (Tiers 2-4) is governed by the True Sovereign Lifecycle.
- The Native Envoy (C++) operates as a Dumb Courier. It does **NOT** invent execution capabilities. It strictly enforces collision rules synced dynamically via libcurl from the Architect.

### The 3-Step Sequence
All deployment operations **MUST** follow this 3-step sequence:
1. **HANDSHAKE**: Execute `quanuxctl predeploy`. The C++ Envoy guarantees compatibility against the local `habitat.json` DNA and dynamic Collision Matrix.
2. **OUT-OF-BAND (OOB) DOWNLOAD**: Execute `quanuxctl deploy`. NATS is solely for signaling. The payload is downloaded out-of-band locally by the C++ Envoy via libcurl (in a detached thread). The Envoy natively verifies the `EVP_sha256` hash and applies POSIX `chmod()` permissions, writing the `.run_<payload>.sh` wrapper.
3. **CONDITIONED EXECUTION**: Execute `quanuxctl -ssh <node> <payload>`. The execution is strictly governed by the Architect over Secure Shell. If steps 1 or 2 fail, the execution wrapper will physically not exist.

### The Affinity Mandate
- **4. THE AFFINITY MANDATE**: Any future agents modifying the C++ Native Envoy are explicitly forbidden from spawning generic `std::thread` or `pthread` instances without immediately applying `pthread_setaffinity_np` to lock them to Core 0 (e.g. `pin_thread_to_core(0)`). The Envoy must never allow the OS scheduler to dictate thread placement on edge nodes, as stray threads will thrash the isolated L3 caches of payload algorithms.
- **WARNING**: Future agents must NEVER modify the C++ Native Envoy to execute payloads directly via `system()` or CNATS callbacks without SSH conditioning. NATS is for telemetry, SSH is for execution.

## Network Physics & Asymmetric Routing Assumptions

- **RULE A (The IPv4 Mandate)**: All Architect-side staging servers (HTTP/OOB) must bind explicitly to IPv4 (`0.0.0.0`), as edge nodes may lack IPv6 routing.
- **RULE B (Control vs. Data Plane)**: NATS (Control Plane) and HTTP (Data Plane) route differently. A successful NATS connection does not guarantee HTTP visibility. Payload URLs must use explicit, routable IPs (e.g., LAN IPs), not localhost or default hypervisor gateways.
- **RULE C (The LTS Baseline)**: The deployment physics and networking routing structures are strictly validated against Ubuntu 22.04 LTS (Jammy Jellyfish). Be aware that different OS versions may introduce breaking changes in their network stack or NAT resolution.
