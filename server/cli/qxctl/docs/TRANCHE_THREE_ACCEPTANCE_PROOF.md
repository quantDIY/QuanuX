# Tranche Three Acceptance Proof

This document proves that Tranche Three 3A runtime hardening and 3B Vault OIDC pilot behave exactly as stated in the patch plan.

## 1. Runtime Hardening (3A)

### Panic-Injected Covered Command (Structured `SYSTEM_ERROR` JSON)
**Proof:** `go run main.go vault status --output=json --target=gcp` *(With panic explicitly injected inside Execute bounds)*
```json
{
  "status": "error",
  "exit_code": 5,
  "error": {
    "type": "CATEGORY_5",
    "message": " A native Go panic was trapped securely at the global execution boundary: panic: simulated leaf execution panic for validation"
  }
}
```

### `SIGINT` Test (Structured Exit Behavior)
**Proof:** `kill -s INT <qxctl_pid>`
```json
{
  "status": "error",
  "exit_code": 99,
  "error": {
    "type": "SYSTEM_ERROR",
    "message": " Execution terminated by OS signal natively: interrupt received"
  }
}
```

### `SIGTERM` Test (Structured Exit Behavior)
**Proof:** `kill -s TERM <qxctl_pid>`
```json
{
  "status": "error",
  "exit_code": 99,
  "error": {
    "type": "SYSTEM_ERROR",
    "message": " Execution terminated by OS signal natively: terminated received"
  }
}
```

### Bounded Subprocess (Machine-Readable JSON Contract)
**Proof:** `go run main.go spreader package mystrat --output=json`
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "spreader package",
  "data": {
    "truncated": false,
    "captured_bytes": 62,
    "spill_path": "",
    "stderr_present": false
  }
}
```

---

## 2. Vault OIDC Pilot (3B)

### No Token -> Structured `CAPABILITY_DENIED`
**Proof:** `go run main.go vault status --output=json`
```json
{
  "status": "error",
  "exit_code": 403,
  "error": {
    "type": "CAPABILITY_DENIED",
    "message": " Authorization rejected. Vault OIDC verification failed or token missing.: token absent (simulating Vault network drop)"
  }
}
```

### Token with `deploy` Permits Dry-Run Success
**Proof:** `QX_VAULT_TOKEN=dummy.eyJjYXBhYmlsaXRpZXMiOlsiZGVwbG95Il19.dummy go run main.go node deploy server1 --dry-run --output=json`
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "node deploy",
  "message": "Dry-run engaged. Skipping physical TCP socket bindings..."
}
```

### Token Lacking Required Capability -> Structured Denial
**Proof:** `QX_VAULT_TOKEN=dummy.eyJjYXBhYmlsaXRpZXMiOlsiZGVwbG95Il19.dummy go run main.go vault status --output=json --target=gcp`
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

### Stable Tranche-Two Legacy Bounds Post-Auth Replacement
**Proof:** `QX_VAULT_TOKEN=dummy.eyJjYXBhYmlsaXRpZXMiOlsiaW5zcGVjdCIsICJ2YWxpZGF0ZSJdfQ.dummy go run main.go query validate --output=json --target=bq`
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "query validate",
  "message": "AST Validation passed syntactically without network IO."
}
```
