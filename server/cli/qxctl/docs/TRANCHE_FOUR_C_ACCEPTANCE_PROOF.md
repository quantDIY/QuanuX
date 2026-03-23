# Tranche 4C Acceptance Proof (Option 3 Stage 1)

## Architecture Configuration
- **Coverage Scope:** `infra apply` is the sole endpoint targeted and requires `CapDeploy`.
- **Audit Basis:** Verified in code that `infra apply` maps correctly to `infra.Apply` and executes mutative infrastructure provisioning, strictly matching the `RiskDangerous` profile.
- **Global Coverage Bypass:** Intentionally retained for all remaining `infra` nodes (`auth`, `destroy`, `execute`, etc) and other execution-core trajectories.

## Acceptance Criteria

### 1. `infra apply` with an underprivileged token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl infra apply --output=json`
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

### 2. `infra apply` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl infra apply --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "qxctl infra apply",
  "message": "Infrastructure deployed natively and immutably."
}
```

### 3. Uncovered node proving the global bypass is intact
**Command:**
`./qxctl infra destroy --target=gcp --output=json`
**Output:**
- **stdout:** empty / untouched
- **stderr:** empty / untouched
- **exit_code:** 0
