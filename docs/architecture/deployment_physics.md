# QuanuX Deployment Physics: The True Sovereign Lifecycle

## Background
The QuanuX distributed architecture mandates a strict bifurcation between the Tier 1 Control Plane (Python) and Tier 2-4 Execution Edge (C++). Deploying trading logic across this boundary requires absolute precision. To preserve C0 spin-loop integrity and avoid kernel starvation, the system employs isolated transport mechanisms.

## The 3-Step Lifecycle
1. **NATS Handshake (`quanuxctl predeploy`)**
   - **Protocol**: Zero latency, request-reply over CNATS.
   - **Physics**: Validates payload requirements against the edge node's local `/etc/quanux/habitat.json` hardware DNA. The C++ Native Envoy enforces dynamic collision boundaries securely and will successfully fire a native NACK for incompatible capabilities (e.g., requesting `onload` on an `ef_vi` mapped edge node).

2. **Out-of-Band Transport (`quanuxctl deploy`)**
   - **Protocol**: Over-the-air `NATS` signaling triggering an HTTP `libcurl` download.
   - **Physics**: NATS is isolated for pure telemetry and signaling. The CLI sends an execution blueprint (URL, hash, capability prefix) via IPC. The Envoy spawns a detached C++ thread to natively download the binary, verify the `EVP_sha256()` hash, and natively apply POSIX `<sys/stat.h> chmod()` execution rights without blocking the primary dispatcher.

3. **Conditioned SSH Executor (`quanuxctl -ssh`)**
   - **Protocol**: Standard Secure Shell.
   - **Physics**: The C++ Envoy acts as a Dumb Courier, blindly writing the execution prefix (e.g., `numactl`, `onload`) into a `.run_<payload>.sh` wrapper. The Architect executes this script via SSH. If the Handshake or OOB transfer failed, the script does not exist, physically gating unauthorized execution.

## Distributed State Management
The hardware collision rules (`COLLISION_MATRIX`) are dynamically synced to the edge nodes at boot time via `libcurl`. Centralized capability tracking prevents distributed state desynchronization. Native Envoys enforce the Architect's current laws without hardcoded assumptions and successfully generate dynamic Handshake NACK/ACK matrix resolutions.

## Network Physics & Asymmetric Routing Assumptions

The Sovereign Edge forces a strict differentiation between signaling and data movement. **Note:** The entire deployment trace, pathing, and network routing isolation strategies have been physically verified against **Ubuntu 22.04 LTS (Jammy Jellyfish)**. Other distributions or LTS versions may exhibit breaking changes in their networking stacks or NAT behaviors, requiring rigorous re-validation.

1. **The IPv4 Mandate**: All Architect-side staging servers handling payload delivery (HTTP/OOB) must explicitly bind to IPv4 (`0.0.0.0`). Edge nodes and raw metal often lack IPv6 routing for internal topologies.
2. **Control Plane vs. Data Plane Asymmetry**: The NATS signaling network (Control Plane on Port 4222) and the external HTTP payload transport network (Data Plane) route differently. A successful NATS Handshake does not guarantee HTTP visibility. Payload URLs must map to explicit, routable IPs (e.g., physical LAN IPs), avoiding localhost or hypervisor default NAT gateways (`10.0.2.2`). Network engineers must configure edge-switch ACLs and UFW to explicitly allow both protocol planes from the Tier 1 Architect.
