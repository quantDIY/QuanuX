# Minimum Viable Rollback Plan

This establishes explicit escape strategies allowing rapid rollback loops reverting execution bounds accurately.

## Files Altered
- `internal/runtime/auth.go` (Added Token mapping arrays and Authorize intercept logic)
- `internal/exec/exec.go` (Added BoundedRun IO buffer mechanics)
- `internal/errors/errors.go` (Added `CategoryCapabilityDenied`)
- `internal/output/ux.go` (Mapped category 403 explicitly into `CAPABILITY_DENIED` arrays)
- `cmd/root.go` (Wired `PersistentPreRunE` securely)
- `cmd/secrets.go`, `cmd/spreader.go`, `cmd/node.go` (Bound explicit command capabilities structure safely)

## Instantly Severing Authorization Checks
1. Navigate to `server/cli/qxctl/cmd/root.go`.
2. Find the `PersistentPreRunE` field inside the `rootCmd`.
3. Replace the `if err := app.Authorize(cmd); err != nil` block with `return nil`.
4. This removes authorization checks and returns the CLI to Tranche One behavior.

## Severing Subprocess Capture Loops
1. Navigate to `server/cli/qxctl/cmd/spreader.go`.
2. Locate the `RunE` function inside `packageCmd`.
3. Replace the `cliExec.BoundedRun` call with `return nil` to disable subprocess capture.
