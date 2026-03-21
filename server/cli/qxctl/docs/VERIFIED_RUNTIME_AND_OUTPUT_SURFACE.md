# Verified Runtime and Output Surface

This document holds the exact structural proofs of the `qxctl` core architecture inside the `quanux-qxctl` branch.

### 1. `runtime.App` Exact Fields

**[VERIFIED]** Source: `server/cli/qxctl/internal/runtime/app.go`
```go
type App struct {
	Cfg *config.Config
	Ctx context.Context
	Out *output.Manager
	
	NATS  providers.JetStreamEngine
	Vault providers.SecretStore
}
```

### 2. Output Manager Exact Methods

**[VERIFIED]** Source: `server/cli/qxctl/internal/output/ux.go`
The `output.Manager` exposes strictly these structural signatures for string emission mapping:
- `func (m *Manager) EmitRaw(msg ...interface{})`
- `func (m *Manager) EmitRawf(format string, msg ...interface{})`
- `func (m *Manager) Print(data interface{}, humanize func() string)`
- `func (m *Manager) Info(message string)`
- `func (m *Manager) Warn(message string)`
- `func (m *Manager) Error(message string)`
- `func (m *Manager) Debug(message string)`
- `func (m *Manager) Log(level, message string)`

### 3. Config & Output Mode Handling

**[VERIFIED]** Source: `server/cli/qxctl/main.go` and `server/cli/qxctl/internal/runtime/app.go`
- Config modes are overridden organically outside Cobra bounding loops utilizing `.ParseOverrides(os.Args[1:])`.
- Output Mode (`quiet`, `json`, `yaml`, `text`) correctly configures the structural state inside `output.NewManager(cfg.Output)`.

### 4. Current Redaction Insertion Points

**[VERIFIED]** Source: `server/cli/qxctl/internal/output/ux.go` and `server/cli/qxctl/internal/security/redact.go`
- `m.EmitRaw()`, `m.EmitRawf()`, `m.Print()`, and `m.Log()` universally wrap the final string payload inside `fmt.Println(security.Redact(raw))`.
- Redaction successfully captures Bearer tokens, `key=/password=/token=` signatures, and Vault sequences natively.

### 5. Current Error-Exit Paths

**[VERIFIED]** Source: `server/cli/qxctl/main.go`
- Trapped exceptions hitting `rootCmd.ExecuteContext(ctx)` bounce cleanly into `qxerrors.Exit(err)`.
- **[WEAK INFERENCE]** `qxerrors.Exit(err)` executes `os.Exit(1)` natively mapping global error strings to stderr.

### 6. Subprocess STDERR/STDOUT Handling

**[STRONG INFERENCE]** Commands utilizing `hashicorp/terraform-exec` or `os/exec` directly push buffers out to OS pipes if not explicitly caught by `io.Writer`. No universal subsystem boundary maps native subprocess logs backwards through `app.Out.EmitRaw()` today.

### 7. Structured Failure Unification Insertion Path

**[PROPOSAL]** The literal least-refactor vector to enforce `{"status":"error"}` JSON payloads resides explicitly inside bridging `qxerrors.Exit(err)`:
1. Deprecate `qxerrors.Exit(err)`.
2. Expand `output.Manager` structurally adding `func (m *Manager) ErrorExit(err error, code int)`.
3. In `main.go`:
   ```go
   if err := root.ExecuteContext(ctx); err != nil {
   	app.Out.ErrorExit(err, 1)
   }
   ```
