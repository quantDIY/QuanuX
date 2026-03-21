# Tranche 4B2 Acceptance Proof

## Architecture Configuration
- **Coverage Scope:** `dashboard start` is the sole endpoint targeted and requires `CapInspect`.
- **Audit Basis:** Verified in code that `dashboard start` strictly executes read-only NATS JetStream connectivity logs and launches the TUI view without performing mutating physical filesystem writes, remote API provisioning, or background state setups.
- **Global Coverage Bypass:** Intentionally retained.

## Acceptance Criteria

### 1. `dashboard start` with a `simulate` token
**Command:**
`QX_VAULT_TOKEN=<simulate_token> ./qxctl dashboard start --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": " Authorization rejected. Caller lacks [inspect] capability required to evaluate structural bounds."
  }
}
```

### 2. `dashboard start` with a valid `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl dashboard start --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "start",
  "message": "Dashboard TUI engine launched successfully in read-only mode."
}
```

### 3. Uncovered command proving the bypass still holds
**Command:**
`./qxctl engine start --target=gcp --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
