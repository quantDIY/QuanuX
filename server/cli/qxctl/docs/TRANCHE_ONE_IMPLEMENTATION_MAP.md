# Tranche-One Implementation Map

This implementation map provides the strict, narrow-scope path to executing the first tranche of the `qxctl` AI-readiness contract. 

**Scope Limits:** This tranche solely implements structural metadata, output envelopes, failing path unification, and manifest generation for a targeted subset of commands. Capability enforcement and full execution-contract rollout are explicitly deferred to Tranche Two.

---

## 1. Command Metadata Registry

**Goal:** Formally define AI execution bounds per command in strict C-style structs.

**Target Files:** 
- `server/cli/qxctl/internal/runtime/metadata.go` (`[PROPOSAL]` New)
- Subset of `server/cli/qxctl/cmd/*.go` constructors (Modified)

**Implementation Steps:**
1. Define the `CommandMetadata` struct spanning `CapabilityClass`, `RiskLevel`, `IsIdempotent`, `SupportsDryRun`, and `RequiresInteractive`.
2. Introduce a `BindMetadata(cmd *cobra.Command, meta CommandMetadata)` helper injecting the serialized JSON struct into `cmd.Annotations["qxctl_metadata"]`.
3. Invoke `BindMetadata` initially upon a strictly defined Tranche-One covered set (e.g., `NewVaultCmd`, `NewQueryCmd`).

## 2. Metadata Completeness Tests

**Goal:** Provide zero-trust architectural enforcement preventing metadata drift via structural `t.Fatal()` bounds.

**Target Files:**
- `server/cli/qxctl/cmd/root_test.go` (Modified)

**Implementation Steps:**
1. Append `TestCommandMetadataCompleteness(t *testing.T)`.
2. Define an explicit tranche-one `CoveredCommands` map (e.g., `{"vault status": true, "query validate": true}`).
3. Traverse the Cobra tree dynamically. If any command in the `CoveredCommands` map triggers `RunE` or `Run` but lacks `cmd.Annotations["qxctl_metadata"]`, the test executes `t.Fatalf()` natively, failing the build pipeline hard.
4. Commands absent from the covered set are explicitly logged as deferred.

## 3. Structured Output Envelope

**Goal:** Enforce unified, parsable JSON schema delivery for machine readers across targeted commands.

**Target Files:**
- `server/cli/qxctl/internal/output/envelope.go` (`[PROPOSAL]` New)
- `server/cli/qxctl/internal/output/ux.go` (Modified)

**Implementation Steps:**
1. Define `OutputEnvelope` standardizing `Status` (success/partial/error), `ExitCode`, `Command`, `Data`, `Message`, and `ErrorDetail`.
2. Append `func (m *Manager) PrintJSON(env OutputEnvelope)` to manage the exact serialization boundary.
3. When `app.Cfg.Output == "json"`, terminal command completion routes into `app.Out.PrintJSON()`. 
4. Output bypassing the manager in Tranche One includes uncaptured subprocess streams (e.g., native `terraform` text) and core runtime panics.

## 4. JSON Failure-Path Unification

**Goal:** Route failing boundaries through a centralized envelope preserving structural integrity.

**Target Files:**
- `server/cli/qxctl/main.go` (Modified)
- `server/cli/qxctl/internal/output/ux.go` (Modified)
- `server/cli/qxctl/internal/errors/errors.go` (Modified)

**Implementation Flow:**
1. **Entry Point:** Errors enter via Cobra's native execution return in `main.go` (`if err := root.ExecuteContext(ctx); err != nil`).
2. **Redirection:** Modify `main.go` to divert `qxerrors.Exit(err)` into a newly created `output.Manager.ErrorExit(err error)` receiver.
3. **Normalization:** `output.Manager.ErrorExit` normalizes the error boundary into the `OutputEnvelope`. 
4. **Exit Codes:** The internal code is cast directly from `err.(*CliError).Category`. For unknown or non-`CliError` values, the struct defaults to exit code `99` mapping a generic `SYSTEM_ERROR`.
5. **Mode Handling:** If `--output=json` is active, the manager exclusively emits the serialized error envelope routing directly to `os.Stdout`. Standard `os.Stderr` channels are explicitly bypassed/suppressed for the handled fault.

## 5. Redaction Boundary Tightening

**Goal:** Prevent structural envelope formatting from decoupling the native regex screening loops over secrets.

**Target Files:**
- `server/cli/qxctl/internal/output/ux.go` (Modified)

**Implementation Steps:**
1. Tranche one redaction is performed exactly on the serialized envelope output boundary.
2. Inside `m.PrintJSON(...)`, execute `security.Redact()` over the final dynamically marshaled string before routing it to the destination `io.Writer`.

## 6. Manifest Seed Generation

**Goal:** Serialize the live binary bounds dynamically to enable safe offline AI verification.

**Target Files:**
- `server/cli/qxctl/cmd/ext/manifest.go` (`[PROPOSAL]` New)
- `server/cli/qxctl/cmd/ext.go` (Modified)

**Implementation Steps:**
1. Create `qxctl ext manifest` command dynamically walking the tree.
2. Scrape `cmd.Annotations["qxctl_metadata"]` specifically for the tranche-one covered set.
3. Emit structurally via `app.Out.PrintJSON(ManifestEnvelope)`.

---

## 7. Tranche-One Completion Conditions

The Tranche One deployment is fully successful exclusively when all of the following bounds are proven true:

- The defined set of Tranche One covered commands successfully bind architectural metadata.
- The `TestCommandMetadataCompleteness` execution natively fails hard (with `t.Fatal()`) if any covered leaf command lacks structural metadata.
- The `json` output mode natively claims ownership of both the success and failure evaluation paths for all tranche-one-covered commands.
- The active redaction boundary flawlessly executes on the serialized JSON output string immediately prior to terminal routing.
- The AI manifest can be flawlessly generated exclusively reflecting the constraints of the live covered metadata set.
