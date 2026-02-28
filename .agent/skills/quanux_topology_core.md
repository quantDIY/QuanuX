---
name: QuanuX Topology Core
description: Absolute domain topology of the QuanuX ecosystem defining Habitats, Nests, and the 5 Tier System.
---
# QuanuX Topology Core Mandate

> **IMMORTAL GUIDANCE**: You must check which Tier a Nest belongs to before writing a single line of code.

## Absolute Nomenclature
- **Habitat**: The physical or virtual OS boundary (e.g., your local desktop, an Ubuntu VM, a bare-metal server). It is the conditioned soil.
- **Nest**: The sovereign logic process deployed into the Habitat (e.g., trading engines, AI models, observation layers). "Payload" is deprecated. A Habitat can host one massive Nest or countless microscopic Nests.

## The QuanuX Tier System (The Topological Law)

### Tier 1: The Server (Command Center)
- **Identity:** The orchestrator and heart of the system.
- **Environment:** Remote cloud server, local workstation, or desktop.
- **Tech Stack:** Hosts web servers, APIs, strategy repositories, broker APIs. Runs the `quanuxctl` CLI (Python). Communicates to the cluster via Python NATS clients (`nats-py`).
- **Rule:** It does not execute kinetic trades. It deploys Nests to Habitats.

### Tier 2: The Polyglot Nests (Flexibility & Intelligence)
- **Identity:** AI strategy labs, observation layers, and standard API execution (TopstepX, IBKR TWS).
- **Environment:** Extremely flexible. Can be deployed remotely or run locally on the Tier 1 desktop without strict OS conditioning.
- **Tech Stack:** C++, Python, Cython, Golang. Heavily integrated with Anaconda.
- **Networking Protocol:** Uses CNATS (`libnats-c`) if compiled in C++ for cluster communication. Uses ZMQ (ZeroMQ) strictly as a lightweight, zero-overhead replacement for CNATS *only* for pure local, single-machine installations.

### Tier 3: Bare-Metal Nests (The 59ns Compute Engine)
- **Identity:** Unforgiving speed. High-speed statistics, order book building.
- **Environment:** Bare-metal Linux, likely exchange colocation. Strictly conditioned Habitats (isolated cores, NUMA tuning).
- **Tech Stack:** Pure, hyper-optimized C++.
- **Networking Protocol:** Strictly CNATS (`libnats-c`) for telemetry and signaling. Uses standard Linux kernel networking (`epoll`, `io_uring`) for market data/execution. It does not cross the threshold into the physical NIC hardware.
- **RESTRICTION:** You are EXPLICITLY FORBIDDEN from attempting to install Python/Anaconda on Tier 3 Habitats.

### Tier 4: The Fiber Nests (The Kinetic Edge)
- **Identity:** Pure hardware warfare. Full HFT.
- **Environment:** Exchange colocation only. Bare-metal Linux.
- **Tech Stack:** Pure C++.
- **Networking Protocol:** CNATS for telemetry. For execution, it completely abandons the Linux OS networking stack. It uses Solarflare network cards and requires kernel bypass technologies specifically `ef_vi` (raw DMA) or `onload`.
- **RESTRICTION:** You are EXPLICITLY FORBIDDEN from attempting to install Python/Anaconda on Tier 4 Habitats.

### Tier 5: The Global Swarm (Future Horizon)
- **Identity:** Interconnected arbitrage across global data centers.
- **Networking Protocol:** Microwave or millimeter-wave networks.
