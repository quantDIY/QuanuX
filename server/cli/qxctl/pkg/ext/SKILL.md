---
name: Extension (Ext) Module
description: Guidelines for operating the qxctl ext package.
---

# `qxctl ext` Architecture & Operations

## Purpose
The `ext` module is a fully covered boundary for managing local Cython expansions and hardware plugins. This is one of the most mature modules in the baseline, enforcing complete coverage natively.

## Relevant Commands
All commands within `ext` operate under strict JSON parameters when `--output=json` is provided.

**Observational (`CapInspect` | `RiskStable`)**:
- `ext list`
- `ext status`
- `ext upgradeable`
- `ext manifest`

**Mutative (`CapDeploy` | `RiskDangerous`)**:
- `ext install`
- `ext clean`
- `ext enhance`
- `ext integrate`
- `ext run`
- `ext start`
- `ext stop`
- `ext uninstall`
- `ext upgrade`

## Capability Enforcement
Because `ext` is fully migrated into the Covered Set, it evaluates HashiCorp Vault tokens securely. Calling any mutative endpoint (like `ext install`) without a valid `deploy` capability will always trigger JSON structure containing `CAPABILITY_DENIED`.

## Manual References
Check the exact flag sets dynamically via `docs/md/qxctl_ext.md`.
