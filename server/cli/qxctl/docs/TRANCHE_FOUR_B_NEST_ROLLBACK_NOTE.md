# Tranche 4B Option 2 Stage 2 (Nest) Rollback Plan

## Files Altered
- `cmd/nest.go`

## Disabling Tranche 4B Target Tracking
If `CapDeploy` limits block verified deployments unintentionally or trigger network connection panics blocking habitat topologies:
1. Open `server/cli/qxctl/cmd/nest.go`.
2. Erase the single `runtime.BindMetadata(...)` initialization rule mapped explicitly onto `dropCmd`.
3. By deleting this isolated execution bind, `nest drop` securely returns into the broader uncovered-command evaluation block. This gracefully resets global deploy access bypassing the constraints specifically for the nest environment without damaging Tranche 4A cryptographic math validations for `node`, `storage`, `dashboard`, or `telemetry`.
