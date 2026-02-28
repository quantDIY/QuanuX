---
name: Dynamic Manifest Management
description: Defines how agents interact with capability_registry.yaml and payload requirements for deploying nodes in the QuanuX Ecosystem.
---
# Dynamic Manifest Management

## Tools
- `quanuxctl predeploy`
- `quanuxctl deploy`

## Context & Rules
- Never invent a capability string. All hardware/software requirements must exist in `manifests/capability_registry.yaml`.
- If a new technology is introduced (e.g., `fpga_route`), the agent must first update the central registry with its definition and `wiring_hook`, then update the Tier manifests, and finally use it in a payload.
- Payload Deployment is strictly a three-step process: 
  1. **NATS Handshake** (`quanuxctl predeploy`) 
  2. **NATS-Triggered Out-of-Band Download** (`quanuxctl deploy`) 
  3. **Conditioned Execution Wrapper via SSH** (`quanuxctl -ssh`)
- The Handshake matches a Payload's `requirements.json` against a Habitat's `/etc/quanux/habitat.json` dynamically.
- NATS is isolated for IPC telemetry only. Payloads are never transferred over NATS; they are downloaded out-of-band by the Native Envoy via libcurl, and Native Envoy natively enforces collision detection (e.g., `onload` vs `ef_vi`) during Handshakes.
