# Tranche 4B Option 2 Stage 3 (Ext) Acceptance Proof

## Architecture Configuration
- **Coverage Scope:** The explicitly evaluated `ext` and `ext_manifest` command paths locally isolating read access.
  - `ext manifest`, `ext list`, `ext status`, `ext upgradeable` structurally require `CapInspect`.
  - The remaining 10 destructive binary file mechanics (`ext install`, `ext run`, `ext stop`...) strictly require `CapDeploy`.
- **Global Coverage Bypass:** Intentionally retained structurally targeting unmapped components.

## Acceptance Criteria

### 1. `ext manifest` with an `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl ext manifest --output=json`
**Output (Excerpt):**
```json
{
  "status": "success",
  "exit_code": 0,
  "data": {
    "version": "1.0",
    "commands": [
      {
        "capability_class": "inspect",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl ext manifest"
      },
      {
        "capability_class": "deploy",
        "risk_level": "dangerous",
        "idempotent": false,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl ext install"
      },
      {
        "capability_class": "deploy",
        "risk_level": "dangerous",
        "idempotent": false,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl nest drop"
      },
      {
        "capability_class": "deploy",
        "risk_level": "dangerous",
        "idempotent": false,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl storage duckdb init"
      },
      {
        "capability_class": "inspect",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl storage scan"
      },
      {
        "capability_class": "inspect",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": true,
        "schema_ref": "qxctl dashboard start"
      },
      {
        "capability_class": "deploy",
        "risk_level": "dangerous",
        "idempotent": false,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl telemetry start"
      },
      {
        "capability_class": "inspect",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl telemetry status"
      }
    ]
  }
}
```

### 2. `ext install` with an `inspect` token
**Command:**
`QX_VAULT_TOKEN=<inspect_token> ./qxctl ext install my-plugin --output=json`
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

### 3. `ext install` with a valid `deploy` token
**Command:**
`QX_VAULT_TOKEN=<deploy_token> ./qxctl ext install my-plugin --output=json`
**Output:**
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "install [name]",
  "message": "Extension built and installed securely."
}
```

### 4. Uncovered command proving the bypass still holds
**Command:**
`./qxctl engine start --target=gcp --output=json`
**Output:**
- **stdout:** empty
- **stderr:** empty
- **exit_code:** 0
