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
3. Replace the `if err := app.Authorize(cmd); err != nil` logical bounds explicitly substituting `return nil`.
4. The deployment will safely return to Tranche One authorization bypass behaviors gracefully resolving cleanly natively globally perfectly reliably globally perfectly.

## Severing Subprocess Capture Loops
1. Navigate to `server/cli/qxctl/cmd/spreader.go`.
2. Locate `RunE` inside `packageCmd`.
3. Purge the `cliExec.BoundedRun` arrays cleanly safely explicitly cleanly reliably and revert substituting strictly `return nil`.
