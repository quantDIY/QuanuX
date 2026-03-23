# QXCTL AI Command Reference

**Context:** Treat this document as the absolute source of truth for an AI agent navigating the QuanuX Native Go CLI (`qxctl`). It defines strictly how to interface with the binary bounds, how capabilities are structured, and what limitations currently remain actively unmigrated (bypassed).

## 1. Native CLI Architecture
The `qxctl` CLI operates exclusively using Cobra structure paths without any `bash` shelling internally.
- Commands are organized uniformly: `./qxctl [module] [action] [arguments] --flags`
- Example: `./qxctl telemetry start`

## 2. Output Shapes & Workflows
When executed natively, standard output is plain text standard out/standard error.
**AI Workflow Requirement:** You must aggressively pass `--output=json` when executing capability-bound endpoints to receive deterministically parseable JSON payloads guaranteeing structured responses.

**Success JSON Envelope:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "qxctl [module] [action]",
  ...
}
```

**Denial JSON Envelope:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": "Authorization rejected..."
  }
}
```

## 3. The Capability Model
Endpoints are strictly protected by standard HashiCorp Vault tokens loaded via the environment variable `QX_VAULT_TOKEN`. The binary evaluates operations mapping to three distinct roles dynamically:

- **`CapInspect` (Safely Observational):** Actions that read memory, fetch status blocks, and compile arrays passively.
- **`CapDeploy` (Mutative & Dangerous):** Actions physically altering external state, injecting logic grids, executing transactions, or modifying the Cython arrays natively. Examples: `spreader deploy`, `infra apply`, `nest drop`, `telemetry start`.
- **`CapValidate` / `CapSecretsRead`:** Granular scopes explicitly restricting environments smoothly. 

If your assigned token lacks `CapDeploy`, mutative interactions will hard-block natively with a 403 `CAPABILITY_DENIED` json envelope instantly.

## 4. The Covered Command Matrix (Hardened Endpoints)
As of the current phase, these operations strictly implement explicit metadata constraints and structured JSON outcomes.
- **`telemetry status`**: `CapInspect` || **`telemetry start/stop`**: `CapDeploy`
- **`dashboard start`**: `CapInspect`
- **`ext list`, `ext status`, `ext upgradeable`, `ext manifest`**: `CapInspect`
- **`ext install`, `ext clean`, `ext enhance`, `ext integrate`, `ext run`**: `CapDeploy`
- **`spreader package`**: `CapSimulate` || **`spreader deploy`**: `CapDeploy`
- **`storage scan`**: `CapInspect` || **`storage init`, `storage attach`, `storage detach`**: `CapDeploy`
- **`vault status`**: `CapInspect`
- **`query validate`**: `CapValidate`
- **`secrets get`**: `CapSecretsRead`
- **`nest drop`**: `CapDeploy`
- **`infra apply`**: `CapDeploy`
- **`vcs commit`**: `CapDeploy`
- **`node deploy`**: `CapDeploy`

## 5. Uncovered Command Caveats (The Global Bypass)
The CLI operates with a legacy fallback bounds parameter. If a command is **NOT** explicitly listed in the section above, it operates **COMPLETELY OUTSIDE** the capability model dynamically bypassing all structural Vault authentication silently.

**High-Risk Bypassed Modules to avoid executing lightly:**
- `topstepx apikey`, `module check`, `orchestra compile`, `deploy deploy`, `lifecycle deploy`, `foundry deploy`, `disable disable`, `enable enable`.
These bypassed nodes execute raw text logs exclusively and completely ignore JSON UI formatting.

## 6. Granular Node Documentation
For exhaustive flag, parameter, and exact usage documentation logically mapped automatically, navigate directly to the `docs/md/` and `docs/man/` directory trees compiled completely inside the `qxctl` space.
