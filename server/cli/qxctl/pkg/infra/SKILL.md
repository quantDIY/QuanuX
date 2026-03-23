---
name: Infra Module Bounds
description: Guidelines for operating the qxctl infra module parameters.
---

# `qxctl infra` Architecture & Operations

## Purpose
The `infra` module is the primary boundary for managing and mutating physical infrastructure limits containing hardware loops, GCP Cloud SQL matrices, and Terraform AST environments locally.

## Relevant Commands
1. **`infra apply`**
   - **Capability/Risk:** `CapDeploy` | `RiskDangerous`
   - **Workflow:** Deploys core terraform or infrastructure templates safely.
   - **Output:** Requires `--output=json` for structured format. Expects `CAPABILITY_DENIED` natively if executing without deploy bounds.
2. **UNCOVERED Bypasses: `infra gcp-sql`, `infra auth`, `infra status`, `infra execute`**
   - **Caveat:** These commands are unmigrated. They skip all capability enforcement and generate unstructured raw text traces. Do not rely on JSON validation arrays natively.

## Cross Checks
- Always confirm target paths via `docs/md/qxctl_infra.md`.
- See `docs/AI_COMMAND_REFERENCE.md` for broader capability logic.
