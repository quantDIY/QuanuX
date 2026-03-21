# Evidence Appendix: Runtime, Output, and Exit 

### 1. `runtime.App` Definition
**[VERIFIED]** Exactly matches DI containment principles avoiding globals.
- **File:** `server/cli/qxctl/internal/runtime/app.go` (Lines 17-24)
```go
type App struct {
	Cfg *config.Config
	Ctx context.Context
	Out *output.Manager
	
	NATS  providers.JetStreamEngine
	Vault providers.SecretStore
}
```

### 2. Output Manager Signatures
**[VERIFIED]** 
- **File:** `server/cli/qxctl/internal/output/ux.go` (Lines 38+)
```go
func (m *Manager) EmitRaw(msg ...interface{}) { ... }
func (m *Manager) EmitRawf(format string, msg ...interface{}) { ... }
func (m *Manager) Print(data interface{}, humanize func() string) { ... }
func (m *Manager) Log(level, message string) { ... }
```
**[VERIFIED]** Missing from `output.Manager` is any error handling structural envelope like `OutputEnvelope` or `ErrorExit()`.

### 3. Exit Handling Mechanism
**[VERIFIED]** `main.go` explicitly routes to `qxerrors.Exit(err)`.
- **File:** `server/cli/qxctl/main.go` (Lines 30-33)
```go
if err := root.ExecuteContext(ctx); err != nil {
	qxerrors.Exit(err)
}
```

**[VERIFIED]** `qxerrors.Exit` handles termination using standard `fmt.Println` rather than `app.Out.EmitRaw()`, completely bypassing output JSON modes natively.
- **File:** `server/cli/qxctl/internal/errors/errors.go` (Lines 41-58)
```go
func Exit(err error) {
	if err == nil {
		os.Exit(0)
	}

	if cliErr, ok := err.(*CliError); ok {
		fmt.Println(theme.FailStyle.Render(fmt.Sprintf("\nCommand Failed: %s\nCategory: %d", cliErr.Message, cliErr.Category)))
		if cliErr.Err != nil {
			fmt.Println(theme.DetailStyle.Render(fmt.Sprintf("Underlying error: %v", cliErr.Err)))
		}
		os.Exit(int(cliErr.Category))
	}

	fmt.Println(theme.FailStyle.Render(fmt.Sprintf("\nFatal System Error: %v", err)))
	os.Exit(99)
}
```

### 4. Statement on JSON Failure Paths
**[VERIFIED]** JSON mode does **NOT** govern failure paths. As proven by the `Exit(err)` body, exceptions are unconditionally dumped to stdout via `fmt.Println()` stripped of any `{"status": "error"}` structure. Furthermore, unrecognized errors return POSIX Exit Code `99` natively, not `1` as previously assumed.
