# Structured Output and Error Envelope Spec (Patch-Planning)

**[CURRENT TRUTH]** Outputs use `app.Out.Print(data)` or `app.Out.EmitRaw(string)`. Errors use `qxerrors.Exit(err)` which drops raw unformatted panic strings to stderr. Exit codes are universally `1` on failure.
**[PROPOSAL]** Adopt a strict structural `OutputEnvelope` inside `internal/output/ux.go` universally mapping standard POSIX exit traps dynamically to AI-readable schema bounds.

## 1. Target Files
- **New/Modified File:** `server/cli/qxctl/internal/output/envelope.go`
- **Modified File:** `server/cli/qxctl/internal/output/ux.go`
- **Modified File:** `server/cli/qxctl/internal/errors/exit.go`
- **Modified File:** `server/cli/qxctl/internal/security/redact.go`

## 2. Distinction: Exit Code vs Application Error Type

**[PROPOSAL]** The **Exit Code** belongs to the OS (e.g., `0`, `2`, `5`) and defines retry loops. The **Application Error Type** belongs to the AI logic (e.g., `AUTHORIZATION_DENIED`, `SUBPROCESS_CRASH`).

## 3. Go Struct Definitions (`envelope.go`)

```go
package output

type OutputStatus string
const (
	StatusSuccess OutputStatus = "success"
	StatusPartial OutputStatus = "partial"
	StatusError   OutputStatus = "error"
)

type OutputEnvelope struct {
	Status  OutputStatus `json:"status"`
	Code    int          `json:"exit_code"`
	Command string       `json:"command"`
	Data    any          `json:"data,omitempty"`
	Message string       `json:"message,omitempty"`
	Error   *ErrorDetail `json:"error,omitempty"`
}

type ErrorDetail struct {
	Type          string `json:"type"`            // e.g. "SUBPROCESS_CRASH"
	Message       string `json:"message"`         // e.g. "Terraform apply failed."
	SubProcessLog string `json:"sub_process_log"` // Raw captured stderr.
}
```

## 4. Manager Methods (`ux.go`)

**[PROPOSAL]** Introduce exact bounded emit chains securely hooking redaction.

```go
// PrintJSON rigidly enforces the OutputEnvelope structural paradigm natively.
func (m *Manager) PrintJSON(env OutputEnvelope) {
	bytes, _ := json.MarshalIndent(env, "", "  ")
	
	// REDACTION INSERTION POINT: Always scrub the final payload before io.Writer
	scrubbed := security.Redact(string(bytes))
	fmt.Fprintln(os.Stdout, scrubbed)
}
```

## 5. Stderr Policy under `--output=json`

**[PROPOSAL]** When `app.Cfg.Output == "json"`, `os.Stderr` MUST be entirely silenced from raw strings dynamically. All crashes must route through `app.Out.ErrorExit()`:

```go
func (m *Manager) ErrorExit(err error, code int, errType string, subLog string) {
	env := OutputEnvelope{
		Status: StatusError,
		Code:   code,
		Error: &ErrorDetail{
			Type:          errType,
			Message:       err.Error(),
			SubProcessLog: subLog, // Passed from Subprocess capture wrapper
		},
	}
	if m.Mode == "json" {
		m.PrintJSON(env)
	} else {
		// Native text fallback for humans
		fmt.Fprintf(os.Stderr, "FATAL [%d] %s: %v\n", code, errType, err)
	}
	os.Exit(code)
}
```

## 6. Subprocess Wrapping Policy

**[PROPOSAL]** Commands like `infra apply` map Terraform. We must trap all Terraform stderr drops dynamically natively inside `bytes.Buffer`.

```go
// Inside pkg/infra/tf_exec.go
var errBuf bytes.Buffer
cmd.Stderr = &errBuf

if err := cmd.Run(); err != nil {
	// Re-route the stdout/stderr collision directly into the structural JSON bound natively
	app.Out.ErrorExit(err, 2, "SUBPROCESS_CRASH", errBuf.String()) 
}
```

## 7. Worked Examples (Current Verified Commands)

### A. `server/cli/qxctl/cmd/infra.go` (infra apply)
**[CURRENT TRUTH]** Streams raw `terraform apply` straight via `os.Stdout`.
**[PROPOSAL]** Suppress `terraform` output when `--output=json`. Capture JSON lines, build standard `OutputEnvelope`. If failed, map `terraform` stderr natively inside `OutputEnvelope.Error.SubProcessLog`.

### B. `server/cli/qxctl/cmd/nest.go` (nest drop)
**[CURRENT TRUTH]** Drops file over SSH. Prints "Success" string.
**[PROPOSAL]** Returns exact payload mapping:
```json
{
  "status": "success",
  "exit_code": 0,
  "command": "nest drop",
  "data": { "target_ip": "10.0.0.5", "binary_checksum": "abcd123" }
}
```
