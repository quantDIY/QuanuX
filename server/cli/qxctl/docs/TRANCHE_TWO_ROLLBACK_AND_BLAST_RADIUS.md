# Tranche Two Rollback and Blast Radius

This document establishes the limits of the changes implemented in Tranche Two and details the manual override strategies required to retract evaluation faults.

## 1. Blast Radius: What Can Break
- **`PersistentPreRunE` Crash:** If the Cobra global hook panics before catching the command state, standard evaluation is interrupted, potentially blocking all CLI execution entirely.
- **Subprocess Hang:** If the buffer pipes inside `[PROPOSAL] internal/exec.BoundedRun` are not closed cleanly on execution faults, `spreader package` could deadlock the underlying Go thread.
- **Metadata Malformation:** If the injected JSON Metadata blocks inside the covered commands contain syntax exceptions, the `Authorize` JSON parser loop will fail initialization.

## 2. Hard Rollback Strategies

### Disabling Enforcement Quickly
To disable authorization constraints instantly, restoring all commands to their legacy states:
1. Open `server/cli/qxctl/cmd/root.go`.
2. Locate `rootCmd.PersistentPreRunE`.
3. Comment out the function body and insert `return nil`.
4. The execution loop completely overrides capability models silently.

### Disabling Subprocess Capture Quickly
To retract `spreader package` buffer constraints:
1. Open `server/cli/qxctl/cmd/spreader.go`.
2. Delete the `ext.BoundedRun()` loop and substitute `return nil`.
3. Outputs will immediately isolate.

### Keeping Uncovered Commands Untouched
The `PersistentPreRunE` wrapper isolates risk through an explicit guard clause:
```go
if _, ok := cmd.Annotations["qxctl_metadata"]; !ok {
    return nil // Skip auth on unmapped commands automatically
}
```
This single check guarantees the 56 legacy branches bypass authorization. Removing this guard clause would globally widen enforcement across all undocumented commands resulting in immediate 403 authorization failures globally.

## 3. Abort Signals
Tranche Two implementation must be aborted mid-flight if:
- Unmapped legacy commands start triggering evaluation faults or HTTP 403 `CAPABILITY_DENIED` errors on routine execution paths.
- Tranche One commands (`vault status`) detach their JSON payloads due to `PreRunE` logic overriding the standard stream handlers.
