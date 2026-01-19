# QuanuX Extensions Registry

Welcome to the **QuanuX Extension Registry**.
This directory contains official bolt-on integrations (Sidecars) that extend QuanuX capabilities without distinctively modifying the Core.

## Architecture (QXP)
Extensions follow the **QuanuX Extension Protocol (QXP)**.
-   **Runtime**: Go (Preferred) or lightweight Node.js/Rust.
-   **Auth**: Managed via **Local API Keys** (no cloud dependencies).
-   **Discovery**: Managed by `quanuxctl`.

## Available Extensions

### [n8n Bridge](./n8n)
-   **Status**: Pilot
-   **Description**: Exposes QuanuX Strategy & Market Data to n8n workflows.
-   **Language**: Go

## Developing an Extension
1.  Create a directory: `extensions/<my-tool>`
2.  Add `extension.yaml` (Manifest).
3.  Build a single binary that listens on a port and accepts `QUANUX_CORE_KEY`.
