# Research: QuanuX Execution Node Architecture

## Executive Summary
The **Execution Node** is a distributed runtime environment for low-latency trading strategies and market data connectors. It extends the current `extensions` framework to run on remote, bare-metal servers while maintaining central orchestration from the QuanuX Server/Client.

## Core Concepts

### 1. Architecture: The "Node Agent"
The Execution Node will be a standalone daemon (likely an evolution of `quanuxctl`) that runs on the target machine.
-   **Role**: Supervisor and Gateway.
-   **Responsibilities**:
    -   **Process Management**: Spawns and monitors strategy/extension processes (Go, Python, C++, Docker).
    -   **Secret Injection**: securely receives secrets from QuanuX HQ and injects them into environment variables.
    -   **Data Brokerage**: Manages a local **NATS Leaf Node** or **ZMQ Proxy** to bridge local low-latency traffic with the remote QuanuX pipeline.
    -   **Telemetry**: Streams stdout/stderr and metrics back to HQ.

### 2. Connectivity & Pipeline: The "Grid"
-   **Topology**:
    -   **The Hub**: A central NATS Cluster (managed by QuanuX Server).
    -   ** The Edge (Execution Nodes)**: Each node runs a **NATS Leaf Node**. This is critical.
        -   *Why Leaf Nodes?* They allow the Execution Node to function offline (local traffic works) and automatically bridge to the Hub when connected. They handle authentication and traffic optimization transparently.
    -   **The Clients**: Desktop and Web apps connect to the Hub, which routes commands to the appropriate Edge Node.
-   **Connection Management**:
    -   **Registration**: `quanux-node register --hub <url> --token <token>` exchanges a one-time token for a permanent NKeys identity creds file.
    -   **Discovery**: Nodes publish heartbeats (`node.<id>.heartbeat`) containing capability tags (e.g., "gpu", "latency:low", "region:ny4").
    -   **Direct Connect**: For ultra-low latency monitoring, the Desktop App can optionally tunnel directly to the Node's ZMQ streams if a direct route (VPN/LAN) exists, bypassing the Hub.

### 3. Polyglot Execution: Native First
We allow the user to choose the runtime best suited for their latency needs:
-   **Bare Metal (Native)**: The default and fastest.
    -   **Go**: `runtime: go`. compiled to binary `go build` and executed directly.
    -   **C++**: `runtime: binary`. Executed directly.
    -   **Python**: `runtime: python`. Uses a specified venv or conda environment on the host.
    -   *Pros*: Zero container overhead, direct access to kernel bypass (onload, etc).
    -   *Cons*: User must manage host dependencies (libs, drivers). We can provide helper scripts (`quanuxctl doctor`) to verify environment health.
-   **Containerized (Optional)**: `runtime: docker`. For ease of deployment when standard latency is acceptable.

### 4. Integration with Skills & QXP
-   **QXP (Quanux Experience/Protocol)**: The Node Agent exposes a standardized control API. `quanuxctl` (local) and `QuanuX Server` (remote) use the same protocol to talk to the Node Agent.
-   **Skills**: We will define a `ManageExecutionNode` skill. This allows the AI Agent to:
    -   "Deploy Strategy X to Node Y"
    -   "Stop all algos on Node Y"
    -   "Fetch logs from Node Y"

## Proposed Implementation Plan

### Phase 1: Local Node Agent (Refactor `quanuxctl`)
Refactor the current `quanuxctl start/stop` logic into a long-running `quanux-node` daemon.
-   It listens for commands (gRPC or NATS).
-   It manages the lifecycle of extensions.

### Phase 2: Remote Connectivity
-   Implement NATS Leaf Node configuration.
-   Implement secure "Link" between HQ and Node (Mutual TLS or Nkeys).

### Phase 3: The "Execution Mode"
-   Enhance `extension.yaml` to support `deploy: remote`.
-   Update UI to show "Nodes" and their running processes.

## Key Questions / Trade-offs
-   **Dependency Management**: For bare metal C++/Python, how do we ensure libraries exist?
    -   *Recommendation*: Use Conda environments (we already use Conda) or Docker.
-   **Latency**: ZMQ vs NATS.
    -   *Recommendation*: Keep ZMQ for the "hot path" (Algo <-> Connector) on the same machine. Use NATS for the "warm path" (UI updates, logging).

