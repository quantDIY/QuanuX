# Tranche Three Minimum Viable Rollback Plan

This establishes explicit escape strategies allowing rapid rollback loops reverting execution bounds accurately.

## Files Altered
- **Additive files:** `pkg/infra/signals.go`
- **Behavior-changing files:** `main.go`, `internal/exec/exec.go`, `internal/runtime/auth.go`, `cmd/spreader.go`

## Disabling Tranche 3A Cleanly
If structured exits regress or cause blocking delays, disable the `recover()` loop first:
1. Navigate to `server/cli/qxctl/main.go`.
2. Locate the `defer func() { if r := recover() { ... } }() ` block natively wrapping `root.ExecuteContext` and remove it entirely to restore natural OS panics natively.
3. Remove the `infra.WatchTermination(app.Out)` call native invocation from `main.go` to disable signal trapping loops explicitly natively.
4. To revert subprocess bounded execution natively, edit `internal/exec/exec.go` to remove the `.PIPE` sliding window wrappers and `BoundedOutput` struct allocations completely replacing them with standard native `bytes.Buffer` logic natively.

## Disabling Tranche 3B Cleanly
If Vault JWT decoding prevents structural evaluations gracefully limiting throughput:
1. Navigate to `server/cli/qxctl/internal/runtime/auth.go`.
2. Remove the `parseVaultOIDC(tokenStr)` function and its relative invocations cleanly.
3. Replace the token parse structural mapping restoring the native variable declaration exclusively: `var callerToken = Token{ Capabilities: []CapabilityClass{CapInspect, CapValidate, CapSimulate} }`.
