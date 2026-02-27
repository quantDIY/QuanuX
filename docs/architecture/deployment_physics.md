# QuanuX Deployment Physics: The True Sovereign Lifecycle

## Background
The QuanuX distributed architecture mandates a strict bifurcation between the Tier 1 Control Plane (Python) and Tier 2-4 Execution Edge (C++). Deploying trading logic across this boundary requires absolute precision. To preserve C0 spin-loop integrity and avoid kernel starvation, the system employs isolated transport mechanisms.

## The 3-Step Lifecycle
1. **NATS Handshake (`quanuxctl predeploy`)**
   - **Protocol**: Zero latency, request-reply over CNATS.
   - **Physics**: Validates payload requirements against the edge node's local `/etc/quanux/habitat.json` hardware DNA. The C++ Native Envoy enforces dynamic collision boundaries securely.

2. **Out-of-Band Transport (`quanuxctl deploy`)**
   - **Protocol**: Over-the-air `NATS` signaling triggering an HTTP `libcurl` download.
   - **Physics**: NATS is isolated for pure telemetry and signaling. The CLI sends an execution blueprint (URL, hash, capability prefix) via IPC. The Envoy spawns a detached C++ thread to natively download the binary, verify the `EVP_sha256()` hash, and natively apply POSIX `<sys/stat.h> chmod()` execution rights without blocking the primary dispatcher.

3. **Conditioned SSH Executor (`quanuxctl -ssh`)**
   - **Protocol**: Standard Secure Shell.
   - **Physics**: The C++ Envoy acts as a Dumb Courier, blindly writing the execution prefix (e.g., `numactl`, `onload`) into a `.run_<payload>.sh` wrapper. The Architect executes this script via SSH. If the Handshake or OOB transfer failed, the script does not exist, physically gating unauthorized execution.

## Distributed State Management
The hardware collision rules (`COLLISION_MATRIX`) are dynamically synced to the edge nodes at boot time via `libcurl`. Centralized capability tracking prevents distributed state desynchronization. Native Envoys enforce the Architect's current laws without hardcoded assumptions.
