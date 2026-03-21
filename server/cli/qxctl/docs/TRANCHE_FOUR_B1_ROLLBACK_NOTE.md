# Tranche 4B1 Rollback Plan

## Files Altered
- `cmd/telemetry.go`

## Disabling Tranche 4B1 Target Tracking
If the capability evaluations for the `telemetry` module block operations incorrectly or fail to resolve valid tokens:
1. Open `server/cli/qxctl/cmd/telemetry.go`.
2. Delete the four `runtime.BindMetadata(...)` definitions mapped against `statusCmd`, `startCmd`, `stopCmd`, and `restartCmd`.
3. This action drops the entire `telemetry` module safely back into the uncovered-command evaluation block. This instantly restores the global execution bypass for telemetry services without modifying or breaking the Tranche 4A cryptographic verification backend for other covered endpoints.
