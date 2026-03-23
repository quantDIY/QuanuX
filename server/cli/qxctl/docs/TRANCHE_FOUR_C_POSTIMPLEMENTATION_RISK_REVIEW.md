# Tranche 4C Post-Implementation Risk Review (Option 3 Stage 1)

## 1. What Remains Bypassed in the `infra` Tree
- The execution binding was isolated strictly to `infra apply`.
- All other 12 sibling endpoints natively bound to the `infra` root command remain executing under the global uncovered-command bypass. 
- The bypassed nodes include: `infra auth`, `infra destroy`, `infra gcp-sql`, `infra execute`, `infra transpile`, `infra validate`, `infra ingest-start`, `infra set-spaces-keys`, `infra set-token`, `infra status`, and `infra table-register`.

## 2. Siblings Drift Analysis
- **Zero Drift Introduced:** `infra apply` has its logic firmly defined in its isolated `&cobra.Command` tree struct internally mapped to `infra.Apply(app.Ctx, target)`. Its physical struct properties (`applyCmd`) do not inherit from nor modify sibling attributes (`authCmd`, `destroyCmd`). 
- **Bypass Intact:** The metadata was bound exclusively to `applyCmd` via `runtime.BindMetadata(applyCmd, ...)`. Because the scanner evaluates bindings strictly leaf-by-leaf iteratively rather than recursively globally passing properties sideways, no sibling inherited `CapDeploy`. Therefore `infra destroy` remains physically untouched structurally continuing exactly as before.

## 3. Structural Stability
- **Success:** Structured JSON emissions have been mapped natively pulling `app.Out.PrintJSON(output.OutputEnvelope{...})` up inside the `applyCmd` execution body natively referencing precisely `cmd.CommandPath()` effectively neutralizing drift variables.
- **Denial:** Structural enforcement operates exactly parallel to previously signed-off CapDeploy execution boundaries (`ext clean`, `storage init`, `node deploy`, etc). Denials trigger `CAPABILITY_DENIED` arrays seamlessly matching the Tranche 1 / Option 2 standards effortlessly.

## 4. Blockers to Broader `infra` Expansion
- The `infra` command tree contains profoundly asymmetric behavior arrays natively combining `CapInspect` style properties (like `infra status` and `infra validate`) with completely mutative `CapDeploy` systems (`infra set-spaces-keys`, `infra destroy`, `infra table-register`).
- Because of this capability spectrum, a broader rollout requires heavy physical auditing of every leaf command iteratively to physically verify which slice requires CapDeploy and which warrants CapInspect securely natively before bindings can be applied globally safely.
