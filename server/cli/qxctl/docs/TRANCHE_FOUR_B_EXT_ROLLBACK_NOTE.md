# Tranche 4B Option 2 Stage 3 (Ext) Rollback Plan

## Files Altered
- `cmd/ext.go`
- `cmd/ext_manifest.go`

## Disabling Tranche 4B Stage 3 Target Tracking
If capability configurations block valid plugin integrations natively running inside execution containers or inadvertently break the programmatic `ext manifest` parsing matrices blocking automated external verification tools:
1. Open `server/cli/qxctl/cmd/ext.go`. Delete the structured `runtime.BindMetadata(...)` implementations mapped explicitly against the 13 `ext` subsystem paths locally initializing subcommands.
2. Open `server/cli/qxctl/cmd/ext_manifest.go`. Drop the explicit `runtime.BindMetadata(...)` initialization linked cleanly against the Cobra struct object strictly returning natively.
3. This returns the `ext` mechanics back inside the functional command-bypass umbrella organically. Secure components like `nest`, `storage`, and `telemetry` remain comprehensively tracked natively.
