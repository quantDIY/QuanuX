# Tranche 4E Acceptance Proof (Option 3 Stage 3)

## Architecture Configuration
- **Coverage Scope:** `spreader deploy` exclusively pulled into the runtime matrix mapping to `CapDeploy`.
- **Audit Basis:** Verified physically that `spreader deploy` calls `spreader.Deploy(...)` mutating external Execution Nodes natively fitting the `RiskDangerous` profile exactly.
- **Global Coverage Bypass:** `spreader package` remains structurally identical under `CapSimulate`. All other `spreader` surfaces (if any existed outside package/deploy) were confirmed to natively remain untouched dynamically securely.

## Acceptance Criteria

### 1. `spreader deploy` with an underprivileged token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl spreader deploy qxs_binary --output=json`
**Output:**
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": " Authorization rejected. Caller lacks [deploy] capability required to execute structural mutative bounds."
  }
}
```

### 2. `spreader deploy` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl spreader deploy qxs_binary --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "qxctl spreader deploy",
  "message": "Spreader engine binary deployed successfully to the Execution Node natively."
}
```

### 3. Uncovered node proving the global bypass is intact
**Command:**
`./qxctl module check --output=json`
**(Chosen module check due to spreader possessing no natively bypassed siblings outside the already-covered packageCmd natively)**
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
