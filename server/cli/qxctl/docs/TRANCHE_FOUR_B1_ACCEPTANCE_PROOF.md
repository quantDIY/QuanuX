# Tranche 4B1 Acceptance Proof

## Acceptance Criteria

### 1. `telemetry start` with an `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl telemetry start --output=json`
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

### 2. `telemetry start` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl telemetry start --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "start",
  "message": "Telemetry service started successfully."
}
```

### 3. `telemetry status` with a valid `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl telemetry status --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "status",
  "message": "Telemetry service status OK."
}
```

### 4. Uncovered command proving the bypass still holds
**Command:**
`./qxctl engine start --target=gcp --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
