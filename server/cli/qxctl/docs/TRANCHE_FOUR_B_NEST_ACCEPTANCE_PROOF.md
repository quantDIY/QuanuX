# Tranche 4B Option 2 Stage 2 (Nest) Acceptance Proof

## Architecture Configuration
- **Coverage Scope:** The explicitly mutative `nest drop` command mapped strictly to `CapDeploy`.
- **Audit Basis:** Verified in code that `nest.go` contains zero observational commands. The single leaf `drop` directly mutates remote habitat topology requiring stringent deploy capabilities securely.
- **Global Coverage Bypass:** Intentionally retained for unconnected commands.

## Acceptance Criteria

### 1. `nest drop` with an underprivileged `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl nest drop target-node --output=json`
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

### 2. `nest drop` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl nest drop target-node --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "drop [target]",
  "message": "C++ Engine deployed successfully."
}
```

### 3. Uncovered command proving the bypass still holds
**Command:**
`./qxctl engine start --target=gcp --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
