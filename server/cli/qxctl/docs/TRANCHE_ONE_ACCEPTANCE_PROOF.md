# Tranche One Acceptance Proof

This document provides definitive, reproducible evidence that all Tranche One implementation boundaries have been satisfied, and the runtime successfully confines the exact covered command set.

## 1. Covered Command Set
The Covered Set for Tranche One is explicitly locked to:
- `qxctl vault status`
- `qxctl query validate`

## 2. Metadata Completeness Test Proof
**Condition:** `cmd/root_test.go` traverses the live tree and `t.Fatalf()` triggers if covered commands lack Metadata.
**Verification:**
```bash
$ go test ./...
?       github.com/QuanuX/qxctl [no test files]
ok      github.com/QuanuX/qxctl/cmd     0.284s
```
*The test natively succeeds because both `vault status` and `query validate` possess `cmd.Annotations["qxctl_metadata"]`, while all other commands register cleanly as `Deferred`.*

## 3. JSON Success Output Proof
**Transcript:** `qxctl vault status --output=json --target=gcp`
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "vault status",
  "message": "Vault status inspection passed natively."
}
```

**Transcript:** `qxctl query validate --output=json --target=bq`
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "query validate",
  "message": "AST Validation passed syntactically without network IO."
}
```

## 4. JSON Failure Output Proof
**Transcript:** `qxctl invalid_command --output=json`
```json
{
  "status": "error",
  "exit_code": 99,
  "error": {
    "type": "SYSTEM_ERROR",
    "message": "unknown command \"invalid_command\" for \"qxctl\""
  }
}
```
*Proof that the native `main.go` trap captures generic Cobra framework errors and accurately routes them into the centralized OutputEnvelope intercept.*

## 5. Redaction Proof on Serialized Target
As proven natively in `internal/output/ux.go`:
```go
func (m *Manager) PrintJSON(env OutputEnvelope) {
	bytes, _ := json.MarshalIndent(env, "", "  ")
	fmt.Println(security.Redact(string(bytes)))
}
```
*Redaction operates strictly upon the `string(bytes)` after complete JSON serialization, guaranteeing the intercept cannot selectively preserve or bypass JSON boundaries.*

## 6. Manifest Generation Proof
**Transcript:** `qxctl ext manifest --output=json`
```json
{
  "status": "success",
  "exit_code": 0,
  "data": {
    "version": "1.0",
    "commands": [
      {
        "capability_class": "validate",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl query validate"
      },
      {
        "capability_class": "inspect",
        "risk_level": "stable",
        "idempotent": true,
        "supports_dry_run": false,
        "requires_interactive": false,
        "schema_ref": "qxctl vault status"
      }
    ]
  }
}
```

## 7. Deferred Out-of-Scope Items
- Capability Enforcement (Reserved for Tranche Two)
- Authn / Token Injection
- Broad expansion of Covered Set to state-mutating targets (`apply`, `deploy`)
- Catching native unstructured panics before runtime ejection.
