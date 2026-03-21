# Minimal Rollback Note

This blueprint isolates the exact coordinates to surgically retract Tranche One if behavioral breakage occurs.

## 1. Files Changed
- `internal/runtime/app.go` (ParseOverrides syntax fix handling `=` delimiters)
- `internal/output/envelope.go` (New)
- `internal/output/ux.go` (Added PrintJSON, ErrorExit hooks natively)
- `internal/runtime/metadata.go` (New)
- `cmd/vault.go` (Modified statusCmd logic)
- `cmd/query.go` (Modified validateCmd logic)
- `cmd/ext.go`, `cmd/ext_manifest.go` (New manifest endpoint and array injection)
- `cmd/root_test.go` (New CI verification script)
- `main.go` (Modified ExecuteContext check pointer)

## 2. Nature of Modifications
Modifications are strictly **additive** dynamically over the pre-existing text execution paths. Text outputs remain inherently untouched for humans.

## 3. Targeted Rollback (JSON Failures)
If unified `JSON failure paths` cause fundamentally unpredictable downstream breakages inside CI pipes organically:
- Revert `main.go` lines 30-32 exclusively back to `qxerrors.Exit(err)`. 
- Executing this single revert explicitly circumvents the `OutputEnvelope` exception routing organically, restoring the text loops flawlessly without deleting structural file arrays.

## 4. Disabling Manifest Generation
`cmd/ext_manifest.go` can be cleanly severed structurally by deleting the `NewManifestCmd(app)` pointer off the array inside `cmd/ext.go` `cmd.AddCommand(...)`. Doing so isolates manifest emission without damaging global metadata registries gracefully.
