# Tranche 4B Option 2 (Storage) Acceptance Proof

## Architecture Configuration
- **Coverage Scope:** The `storage` module commands.
  - `storage scan` requires `CapInspect`.
  - `storage duckdb attach / detach / init` structurally require `CapDeploy`.
- **Global Coverage Bypass:** Intentionally retained for uncovered commands outside `storage` safely.

## Acceptance Criteria

### 1. `storage scan` with an `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl storage scan --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "scan",
  "message": "Connected storage array telemetry scanned properly natively."
}
```

### 2. `storage duckdb init` with an `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl storage duckdb init /tmp/t.db --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": " Authorization rejected. Caller lacks [deploy] capability required to evaluate structural bounds."
  }
}
```

### 3. `storage duckdb init` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl storage duckdb init /tmp/t.db --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "init [path]",
  "message": "DuckDB database file initialized natively on disk."
}
```

### 4. Uncovered command proving the bypass still holds
**Command:**
`./qxctl engine start --target=gcp --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
