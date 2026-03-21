# Tranche Two Acceptance Criteria

This document defines the definitive success conditions required to validate and close Tranche Two.

The Tranche Two rollout is considered fully complete exclusively when all of the following verifications are demonstrated:

1. **Covered Commands Require Capability Metadata**
   - The expanded array (`vault status`, `query validate`, `secrets get`, `spreader package`, `node deploy`) must be strictly bound with parsed metadata.

2. **[PROPOSAL] Structured Denial Responses**
   - Executing a covered command without possessing the matched `CapabilityClass` aborts the execution strictly at pre-execution evaluation within `rootCmd.PersistentPreRunE`.
   - The output conforms symmetrically to the `app.Out.ErrorExit()` envelope design formatted under category 403 `CAPABILITY_DENIED`.

3. **`--output=json` Governs Denial Paths**
   - Any capability-rejected execution explicitly yields valid, parsable JSON outputs instead of falling back to terminal logs when `--output=json` is provided into the root context.

4. **[PROPOSAL] Dry-Run Downgrade Execution**
   - Executing `qxctl node deploy --dry-run` with a token possessing strictly `simulate` capabilities structurally succeeds evaluation.
   - Executing `qxctl node deploy` without `--dry-run` utilizing the exact same token fails with a 403 evaluation error.

5. **Subprocess Capture Validation**
   - The single bounded experiments target, `qxctl spreader package`, must invoke `os/exec.Cmd` cleanly.
   - Normalizing subprocess execution strings accurately maps output exclusively through the JSON envelope `Data` or `Error.SubProcessLog` key values. Output must not bypass to the console.

6. **Uncovered Command Isolation**
   - Legacy commands (e.g., `infra auth`, `nest drop`) continue executing identically as they did in Tranche One without experiencing metadata crashes or authorization blocks, cleanly bypassing `PersistentPreRunE`.

7. **Tranche-One Command Stability**
   - `vault status` and `query validate` continue to execute properly through the new path successfully without losing their Tranche One output stability.
