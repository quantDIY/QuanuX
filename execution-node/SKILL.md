---
name: manage-execution-node
description: Guide for orchestrating QuanuX High-Performance Execution Nodes.
---

# Execution Node Architecture & Management

The **QuanuX Execution Node** is a bare-metal supervisor designed for low-latency trading. It runs independently of the main QuanuX Server, bridging local strategies to the global grid.

## Architecture Overview

### 1. The "Grid" Topology
-   **Hub**: The central QuanuX Server (NATS Cluster).
-   **Edge**: Execution Nodes running `quanux-node` (Go).
-   **Leaf Node**: Each Execution Node runs an embedded **NATS Leaf Node**.
    -   This provides an "Always-On" local message bus.
    -   It transparently bridges traffic to the Hub when internet is available.
    -   Strategies connect to `localhost:7422` (Leaf) instead of the remote Hub.

### 2. Hybrid Data Pipeline
To achieve sub-millisecond latency, we use a hybrid transport model:
-   **Hot Path (Market Data -> Strategy)**:
    -   Uses **ZMQ (PUB/SUB)** or **Shared Memory**.
    -   *Why?* Avoids the localized serialization overhead of the mesh for high-frequency tick data on the same machine.
-   **Warm Path (Telemetry / Orders / UI)**:
    -   Uses **NATS (JetStream)** via the Leaf Node.
    -   *Usage*: Order entry, position updates, logs, and heartbeats.

## 3. The Dual-Engine Philosophy (Graduation Workflow)
QuanuX supports two distinct runtimes for strategies. Agents should guide users through this lifecycle:

### Phase 1: Research & Prototyping (Python/Go)
*   **Engine**: `server/strategies` (Python) or `execution-node/cmd` (Go).
*   **Goal**: Fast iteration, heavy use of ML/Pandas, Backtesting in Strategy Builder.
*   **Indicators**: Uses `import quanux_indicators` (Python Bindings).
*   **Pros**: Easy to write, rich ecosystem.
*   **Cons**: Global Interpreter Lock (GIL), latency overhead.

### Phase 2: Live Production / HFT (C++)
*   **Engine**: `execution-node/cpp` (C++20).
*   **Goal**: Microsecond latency, zero-copy networking, "Packaged" deployment.
*   **Indicators**: Uses `#include "quanux/indicators/..."` (Native Headers).
*   **Workflow**:
    1.  Verify logic in Phase 1.
    2.  **Port** the "Glue Code" to C++ (The math remains identical because it uses the same underlying C++ library).
    3.  Compile into a standalone binary/shared object.
    4.  Deploy to the Execution Node.

## 4. Agent Guidelines: Orchestrating Strategies

As an AI Agent, you can manage these nodes using the standard QuanuX Protocol.

### Deployment Model
Strategies are **Child Processes** managed by the Supervisor.
1.  **Binaries**: Go/C++ compiled binaries.
2.  **Scripts**: Python scripts (run in a pre-configured VirtualEnv).
3.  **Docker**: Containers (optional, for dependencies).

### Common Actions

#### Monitoring Nodes
Subscribe to the heartbeat topic to see active nodes:
`nats sub node.*.heartbeat`

#### Deploying a Strategy
*(Future Capability)*
1.  Upload the binary/script to the node via NATS Object Store.
2.  Send a `Spawn` command:
    ```json
    Topic: node.<id>.spawn
    {
      "cmd": "./my-strategy",
      "args": ["--symbol", "ESH6"],
      "env": {"RISK_LIMIT": "5000"}
    }
    ```

#### Fetching Logs
Tail the standard output of a specific strategy:
`nats sub node.<id>.process.<pid>.logs`

## Troubleshooting
-   **Node Offline?**: Check if the NATS Leaf Node is running (`ps aux | grep nats-server`).
-   **Connectivity?**: Verify the `hub.url` in `~/.quanux-node/config.yaml`.

## Installation & Deployment

### Quick Bootstrap (Pull)
Run this on a fresh Ubuntu/Debian server to install the node and systemd service:

```bash
curl -sL https://quanux.io/install-node | sudo bash -s -- --hub nats://hub.quanux.io:4222 --token <YOUR_TOKEN>
```

### Manual Service Management
If installed via script, the node runs as a systemd service:

-   **Status**: `systemctl status quanux-node`
-   **Logs**: `journalctl -u quanux-node -f`
-   **Restart**: `systemctl restart quanux-node`
