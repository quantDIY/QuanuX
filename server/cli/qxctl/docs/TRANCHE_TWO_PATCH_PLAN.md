# Tranche Two Patch Plan

This document defines the sequential file modifications required to implement Tranche Two capability boundaries and subprocess buffering.

## 1. Dependency Order for Patches
1. **Auth Structs:** Define the `[PROPOSAL]` Token struct and `[PROPOSAL]` `Authorize` evaluation block.
2. **Cobra Interception:** Wire `PersistentPreRunE` on the Root command.
3. **Subprocess IO:** Build the `[PROPOSAL]` buffered `os/exec` wrapper for a single command.
4. **Target Migration:** Apply metadata to the exactly 3 newly expanded covered commands (`secrets get`, `spreader package`, `node deploy`).

## 2. Enforcement Insertion Path (`cmd/root.go`)
- **[PROPOSAL] Action:** Add `PersistentPreRunE` to `rootCmd`.
- **[PROPOSAL] Logic:** 
  1. Retrieve `cmd.Annotations["qxctl_metadata"]`.
  2. If absent, `return nil` (bypassing enforcement entirely for legacy commands).
  3. If present, parse the JSON string back into `runtime.CommandMetadata`.
  4. Call `app.Authorize(cmd, metadata)`.
  5. If `Authorize` returns a capability error, intercept via `app.Out.ErrorExit(err)` carrying exit code 403.

## 3. Denial Output Path (`internal/errors/errors.go` & `internal/runtime/auth.go`)
- **[CURRENT TRUTH]** `output.Manager.ErrorExit` currently maps standard `CliError` fields.
- **[PROPOSAL] Action:** Define `ErrCapabilityDenied = 403` inside `internal/errors`.
- **[PROPOSAL] Action:** Ensure the `Authorize` function constructs a `CliError` holding this category so `ErrorExit` formats the JSON denial envelope identically to Tranche One structures.

## 4. Exact Dry-Run Downgrade Behavior
- **Target File:** `internal/runtime/auth.go` (`[PROPOSAL]` New)
- **[PROPOSAL] Logic:**
  Inside `app.Authorize(cmd, metadata)`:
  ```go
  requiredClass := metadata.CapabilityClass
  
  if metadata.SupportsDryRun {
      isDryRun, err := cmd.Flags().GetBool("dry-run")
      if err == nil && isDryRun {
          requiredClass = runtime.CapSimulate // Downgrade structurally
      }
  }
  ```
  The Token is then evaluated against `requiredClass`.

## 5. Bounded Subprocess Capture Target
- **Target Command:** `spreader package` (The single bounded subprocess-capture experiment for this wave).
- **Target File:** `internal/exec/exec.go` (`[PROPOSAL]` New) and `cmd/spreader.go` (`[CURRENT TRUTH]`)
- **[PROPOSAL] Action:** 
  1. Create `BoundedRun(ctx context.Context, cmd *exec.Cmd) (stdout string, err error)`.
  2. Map `cmd.Stdout` and `cmd.Stderr` to local `bytes.Buffer` instances inside `BoundedRun`.
  3. Modify `spreader package` to invoke a stub bash script via `BoundedRun` instead of `return nil` to rigorously prove the capture mechanism inside the CI without touching heavy external dependencies. Broad subprocess control is explicitly excluded from this tranche.

## 6. Target Files and Modifications
- **`internal/runtime/auth.go`** `[PROPOSAL]`: Add Token injection struct and `Authorize` logic. (Additive)
- **`internal/exec/exec.go`** `[PROPOSAL]`: Add `BoundedRun` buffered subprocess loop. (Additive)
- **`cmd/root.go`** `[CURRENT TRUTH]`: Modify `rootCmd` to implement `PersistentPreRunE`. (Behavior-changing for Covered Set only).
- **`cmd/secrets.go`** `[CURRENT TRUTH]`: Add `runtime.BindMetadata` to `getCmd`. (Additive metadata).
- **`cmd/node.go`** `[CURRENT TRUTH]`: Add `runtime.BindMetadata` to `deployCmd` enabling `SupportsDryRun`. (Additive metadata).
- **`cmd/spreader.go`** `[CURRENT TRUTH]`: Add `runtime.BindMetadata` to `packageCmd` and inject `ext.BoundedRun` execution logic. (Behavior-changing for local node bounds).
