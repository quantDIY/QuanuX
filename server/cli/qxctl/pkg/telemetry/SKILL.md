---
name: Telemetry Module
description: Guidelines for operating the qxctl telemetry package.
---

# `qxctl telemetry` Architecture & Operations

## Purpose
The `telemetry` module controls the native OpenTelemetry and OpenMetrics ingestion streams across the execution paths. It is fully covered under the capability bounds.

## Relevant Commands
All commands within `telemetry` output strict JSON structures when requested.

**Observational (`CapInspect` | `RiskStable`)**:
- `telemetry status` (Verifies mesh health).

**Mutative (`CapDeploy` | `RiskDangerous`)**:
- `telemetry start`
- `telemetry stop`
- `telemetry restart`

## Interaction Requirements
- Execute with `--output=json` to receive structured OutputEnvelopes.
- Mutative changes mandate that the `QX_VAULT_TOKEN` evaluates with `CapDeploy`.
- As a fully covered node, there are zero active bypasses inside this module's boundary.

## Manual References
For argument and environment limits, browse the generated AST arrays natively at `docs/md/qxctl_telemetry.md`.
