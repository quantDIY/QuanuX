# Execution Contract Patch Order

To refactor `qxctl` safely into an AI-capable control-plane grammar without detonating the live Cython loop hooks natively, the implementation MUST follow exactly these 10 atomic patch steps.

---

### Step 1: Implement `runtime.CommandMetadata`
- **Target File:** `internal/runtime/metadata.go`
- **Blast Radius:** `Zero`. Purely additive structs and string mappings natively.
- **Rollback Risk:** None. Safe pre-launch natively.

### Step 2: Implement `OutputEnvelope` and `PrintJSON`
- **Target File:** `internal/output/envelope.go`, `internal/output/ux.go`
- **Blast Radius:** `Low`. Exposes new `.PrintJSON` method natively, does not override legacy `.EmitRaw` immediately.
- **Rollback Risk:** None. Safe pre-launch natively.

### Step 3: Global Trap `ErrorExit`
- **Target File:** `internal/output/ux.go`, `main.go`
- **Blast Radius:** `Medium`. Diverts `qxerrors.Exit(err)` into the exact structural JSON path organically.
- **Prerequisites:** Steps 1 & 2.
- **Safe Pre-launch:** Yes. Will simply change stdout colors/formatting on crashes dynamically.

### Step 4: Map Command Metadata onto Constructors (Phase A - Non-Destructive)
- **Target File:** `cmd/*.go`
- **Blast Radius:** `Low`. Implements `runtime.BindMetadata()` exclusively dropping strings into `cmd.Annotations`. 
- **Rollback Risk:** None. Cobra ignores alien map keys organically.

### Step 5: Enforce Command Metadata Tests
- **Target File:** `cmd/root_test.go`
- **Blast Radius:** `Zero`. Blocks CI dynamically if `BindMetadata` is forgotten on future merges.

### Step 6: Define `SessionToken` and Env Mapper
- **Target File:** `internal/security/session.go`, `internal/runtime/app.go`
- **Blast Radius:** `Low`. Bootstraps dummy AI tokens via `$QUANUX_AI_CAPABILITIES` exclusively inside `runtime.New()`. 
- **Prerequisites:** None.

### Step 7: Subprocess Wrap `terraform` Stderr
- **Target File:** `pkg/infra/apply.go`, `pkg/infra/auth.go`
- **Blast Radius:** `Medium`. Captures `cmd.Stderr = &buf` locally replacing OS leaks natively avoiding unstructured pollution.
- **Safe Post-Launch:** Yes, requires manual testing mapping TF apply delays securely.

### Step 8: `PreRunE` Authorization Hook
- **Target File:** `internal/runtime/auth.go`, `cmd/*.go`
- **Blast Radius:** `High`. Drops `PreRunE: app.AuthorizeAndPrepare()` onto all 34 commands. 
- **Prerequisites:** Steps 4 & 6.
- **Safe Pre-launch:** Yes, BUT must default to "Allow All" if `$QUANUX_AI_CAPABILITIES` is empty to preserve human operability natively.

### Step 9: Develop `qxctl cluster manifest`
- **Target File:** `cmd/ext/manifest.go`
- **Blast Radius:** `Low`. Reads `Annotations` and prints JSON dynamically cleanly decoupled.
- **Prerequisites:** Step 4.

### Step 10: CI Pipeline Drift Guard
- **Target File:** `.github/workflows/qxctl-ai.yml` (or local Makefile)
- **Blast Radius:** `Low`. Runs `go test` and `qxctl cluster manifest` generating the `.quanux/agent_manifest.json` cache per PR dynamically natively.
- **Safe Post-launch:** Yes.
