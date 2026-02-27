# Sovereign Edge: QuanuX Telemetry Workflow

## 1. The Tiered Bifurcation
QuanuX operates on a strictly bifurcated architecture separating the **Control Plane** from the **Execution Edge**.

### Tier 1 (The Nutrient Layer)
- **Role**: General-purpose orchestration, Python backtesting, REST gateways, Data Science.
- **Shell**: The **Fast-Nerve** Watchdog (Python/FastAPI).
- **Environment**: Full Anaconda suite, OS scheduler flexibility. Jitter is acceptable.

### Tiers 2, 3, 4 (The Kinetic Edge)
- **Role**: High-Frequency Trading (HFT), Institutional FIX, Direct Market Access (EF_VI).
- **Shell**: The **Native Envoy** (C++20).
- **Environment**: Bare-metal. Python and ASGI event loops are banned. Latency overhead must be exactly zero. 

## 2. The Physics of the Warm Core
To maintain zero response latency, the CPU cores executing trading and telemetry tasks on Tiers 2-4 *must never enter a sleep state* (e.g., C1, C3, C6).

The **Native Envoy** accomplishes this via two mechanisms:
1. **Thread Affinity (`pthread_setaffinity_np`)**: The Envoy aggressively pins its main thread to Core 0 (Management Core), ensuring it never context-switches onto isolated trading cores (e.g., Cores 3, 5, 7).
2. **The Spin-Wait Loop (`_mm_pause()`)**: Instead of yielding to the OS scheduler via `sleep()`, the Envoy executes an infinite `while(true)` spin-loop containing the `_mm_pause()` Intel intrinsic. This prevents the OS from downclocking the core, avoids speculative execution branch penalty burn, and keeps the L1/L2 cache intensely hot for instantaneous NATS message processing.

## 3. Remote Telemetry Management
Because Tier 2-4 nodes lack SSH and Python REST APIs, telemetry operations (like `systemctl restart`) must be issued centrally.

The Architect node uses the Python `quanuxctl telemetry` CLI:
```bash
quanuxctl telemetry restart --node edge-tx-01 --service envoy
```
This encapsulates a JSON formatted NATS message pointing to `QUANUX.NODE.CMD.edge-tx-01`. The Native Envoy catches the packet from the pre-warmed network buffer.

### The Kamikaze ACK & Sudoers Gasket

There are two critical physics mechanisms applied when the Envoy acts on a telemetry command to restart or stop:

1. **The Sudoers Gasket**: The Native Envoy runs as the local user (often non-root). Normally, `systemctl restart` hangs, waiting for a password prompt. During initialization, a hyper-specific `/etc/sudoers.d/quanux-envoy` file is injected to explicitly grant `NOPASSWD` access *only* for the daemon's `start/stop/restart` binaries.
2. **The Kamikaze ACK Sequence**: If the Envoy successfully executes `system("sudo systemctl restart quanux-envoy")`, it immediately receives a `SIGTERM` from the OS. Any pending NATS network buffers generated after this command are destroyed. In order to acknowledge the telemetry command back to the Architect without creating a ghost/zombie loop on the Central Control Dashboard, the Envoy must:
    * Construct the acknowledgment JSON.
    * Publish it to `QUANUX.NODE.STATUS`.
    * Synchronously call `natsConnection_Flush(conn)` to force the TCP buffers out to the wire.
    * Execute the `system()` call natively.

This combination guarantees an isolated, jitter-free execution daemon that remains fully and securely controllable out-of-band via the NATS framework.
