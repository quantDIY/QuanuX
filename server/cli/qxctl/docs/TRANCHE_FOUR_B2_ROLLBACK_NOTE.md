# Tranche 4B2 Rollback Plan

## Files Altered
- `cmd/dashboard.go`

## Disabling Tranche 4B2 Target Tracking
If the capability evaluations for the `dashboard` module block valid token operations or incorrectly crash the NATS JetStream viewer:
1. Open `server/cli/qxctl/cmd/dashboard.go`.
2. Delete the `runtime.BindMetadata(...)` struct block appended against `startCmd`.
3. This action drops the `dashboard` module back down into the uncovered-command evaluation path. This instantly restores the global execution bypass for the dashboard environment without stripping the Tranche 4A cryptographic constraints mapping `telemetry` or `node deploy`.
