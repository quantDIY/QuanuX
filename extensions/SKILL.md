---
name: quanux-extension-standards
description: The authoritative "School of Architecture" for building QuanuX Extensions (Sidecars). Enforces the QXP protocol, Go runtime preference, and privacy-first security model.
metadata:
  version: 1.0.0
  author: QuanuX
  priority: 10
---

# QuanuX Extension Standards (QXP)

**"Bolt-on, don't Build-in."**

This skill defines the architecture for extending QuanuX. We prefer **Sidecar Processes** over monolithic plugin systems. This ensures isolation, stability, and polyglot capability, though **Go** is the standard.

## 1. The "Sidecar" Philosophy

An extension is a standalone binary that runs alongside the Core Server.
-   **Isolation**: If the extension crashes, Core survives.
-   **Performance**: Extensions perform heavy lifting (websocket streaming, encoding) outside the Python GIL.
-   **Privacy**: Extensions run locally. No data leaves the machine unless the extension explicitly does so (and the user configured it).

## 2. Directory Structure

All extensions reside in the root `/extensions` directory.

```text
/extensions
  /my-extension
    /cmd          # (Optional) if complex
    main.go       # Entrypoint (Go)
    extension.yaml # Manifest
    README.md
```

## 3. The Manifest (`extension.yaml`)

Every extension MUST have a manifest.

```yaml
name: "sierra-chart-bridge"
display_name: "Sierra Chart Connector"
version: "0.0.1"
runtime: "go" # or "node", "python" (Go preferred)
command: "./dist/sierra-bridge" # Relative to extension dir
  - "market.data.read"
  - "strategy.execute"
env:
  - "QUANUX_BRIDGE_KEY" # Injected by quanuxctl
upstream_repo: "https://github.com/my/repo.git" # (Optional) Enable Package Management
```

## 4. Package Management (Lifecycle)

Extensions can opt-in to managed upgrades by defining `upstream_repo` in `extension.yaml`.

-   **Versioning**: `quanuxctl` will query `git ls-remote --tags` on the upstream repo.
-   **Build Script**: `quanuxctl` passes `QUANUX_EXT_VERSION` environment variable to `build.sh`.
    -   Your `build.sh` MUST prioritize this variable over hardcoded versions.
    -   Example: `VERSION=${QUANUX_EXT_VERSION:-"v1.0.0"}`.
-   **Commands**: This enables `quanuxctl upgrade`, `upgradeable`, and `install -v`.

## 5. Communication Protocol

### Inbound (Core -> Extension)
-   Extensions SHOULD expose an HTTP or gRPC server.
-   They `LISTEN` on a configurable port (default range 9000-9100).

### Outbound (Extension -> Core)
-   Extensions connect to QuanuX Core via WebSocket or HTTP.
-   **Auth**: Use `QUANUX_BRIDGE_KEY` (injected via ENV) to authenticate with Core.

## 5. Security & Secrets

-   **NO Hardcoded Creds**: Never store API keys in code.
-   **Keyring**: Use the System Keyring via `quanuxctl` or `server/security/secrets.py`.
-   **Injection**: Secrets are injected as Environment Variables at runtime.

## 6. Implementation Guide (Go)

New extensions should use the standard Go layout:

```go
package main

import (
    "net/http"
    "os"
    "log"
)

func main() {
    // 1. Read Config (Env Vars)
    bridgeKey := os.Getenv("QUANUX_BRIDGE_KEY")
    if bridgeKey == "" {
        log.Fatal("QUANUX_BRIDGE_KEY required")
    }

    // 2. Setup Server
    mux := http.NewServeMux()
    mux.HandleFunc("/health", healthHandler)

    // 3. Start
    port := os.Getenv("PORT")
    if port == "" { port = "9000" }
    log.Fatal(http.ListenAndServe(":"+port, mux))
}

## 7. Remote Connectivity & Port Forwarding

**Protocol**: Extensions MUST support remote deployments where the Target App (e.g. Sierra Chart) is on a different machine than QuanuX Core.

-   **Configurable Host/Port**:
    -   NEVER hardcode `localhost`.
    -   ALWAYS accept `QUANUX_<NAME>_HOST` and `QUANUX_<NAME>_PORT`.
-   **Tunneling Support**:
    -   This design explicitly supports **SSH Tunneling** (e.g. `ssh -R`) and **Reverse Proxies**.
    -   Example: QuanuX (Cloud) -> Connects to `localhost:11099` (Tunneled) -> User PC (Sierra Chart).
    -   Extensions must handle connection drops gracefully (reconnect logic) to support tunnel restarts.

```

## 8. Specialized Extensions

- [Rithmic Integration](rithmic/SKILL.md) - Official Protobuf Implementation & Developer Guide.
