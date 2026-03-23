# Tranche 4D Acceptance Proof (Option 3 Stage 2)

## Architecture Configuration
- **Coverage Scope:** `vcs commit` is the sole endpoint targeted and natively requires `CapDeploy`.
- **Audit Basis:** Verified in code that `vcs commit` executes filesystem mutations (writing local git block chains), strictly aligning to `RiskDangerous`.
- **Global Coverage Bypass:** Intentionally comprehensively retained for all remaining `vcs` nodes (`status`, `clone`, `connect`, `publish`, `push`, `setup`, `sync`).

## Acceptance Criteria

### 1. `vcs commit` with an underprivileged token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl vcs commit -m "update" --output=json`
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

### 2. `vcs commit` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl vcs commit -m "update" --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "qxctl vcs commit",
  "message": "Local VCS state formally committed natively."
}
```

### 3. Uncovered node proving the global bypass is intact
**Command:**
`./qxctl vcs status --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
