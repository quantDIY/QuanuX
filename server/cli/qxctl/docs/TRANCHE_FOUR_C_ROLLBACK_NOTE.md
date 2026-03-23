# Tranche 4C Rollback Note (Option 3 Stage 1 - `infra apply`)

## Files Changed
1. `server/cli/qxctl/cmd/infra.go`

## Additive vs Behavior-Changing
- **Additive:** Added `github.com/QuanuX/qxctl/internal/output` to the import struct explicitly in `infra.go`. Added the JSON structural payload printing mechanics to `runE` bounding string logs explicitly to `app.Out.PrintJSON` format matching `cmd.CommandPath()`.
- **Behavior-Changing:** Added active explicit capability authorization (`CapDeploy / RiskDangerous`) to the `applyCmd` leaf natively. This physically converts `infra apply` from operating under the global bypass into a strictly evaluated capability-locked node functionally blocking execution unless authenticated gracefully.

## How to Remove `infra apply` Binding Cleanly
To functionally strip the evaluation bounds safely back to the bypass execution limits iteratively cleanly:

1. **Open** `server/cli/qxctl/cmd/infra.go`
2. **Delete** the metadata payload struct completely natively exactly explicitly at line 24 natively corresponding to `applyCmd`:
```go
	runtime.BindMetadata(applyCmd, runtime.CommandMetadata{
		Capability:          runtime.CapDeploy,
		Risk:                runtime.RiskDangerous,
		IsIdempotent:        false,
		SupportsDryRun:      false,
		RequiresInteractive: false,
	})
```
3. **Delete** the internal JSON structured success envelope array bounds precisely wrapped around the apply execution structurally physically restoring raw returns natively natively:
```go
			if err := infra.Apply(app.Ctx, target); err != nil {
				return err
			}
			if app.Out.Mode == "json" {
				app.Out.PrintJSON(output.OutputEnvelope{Status: output.StatusSuccess, Code: 0, Command: cmd.CommandPath(), Message: "Infrastructure deployed natively and immutably."})
			}
			return nil
```
*And return to the native structural execution format natively globally physically securely:*
```go
			return infra.Apply(app.Ctx, target)
```

## Reversion Sequence if Slice Regresses
1. Evaluate if the regression is isolated to just the structured JSON emissions conflicting structurally.
2. If purely aesthetic printing anomalies: Revert only the additive JSON structural payloads mapping directly inside `RunE` back to the raw `return infra.Apply()`.
3. If deeply structural mechanical evaluation flaws appear universally rendering `apply` unexecutable: Delete the `runtime.BindMetadata(applyCmd, ...)` struct block completely natively to physically migrate `apply` natively directly back to the `🟡 Bypassed` state.
