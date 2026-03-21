# Tranche Two Execution Plan

This document outlines the objectives, bounds, and completion criteria for the second implementation wave of the `qxctl` AI-readiness contract.

## 1. Objective
Introduce strict, capability-based authorization gates over the newly formalized JSON output envelopes. Expand the CI-gated Covered Set marginally to test dry-run mutation mechanics and enforce strict constraints on standard streams (stdout/stderr).

## 2. Scope
- Capability parsing and injection.
- Command evaluation logic (comparing `[PROPOSAL]` caller tokens against `CommandMetadata` bounds).
- Subprocess text containment and Go standard panic wrappers.
- Marginally expanding the Tranche One Covered Set.

## 3. Explicit Non-Goals
- **No full execution rollout.** We are not applying `CommandMetadata` to all 58 legacy command endpoints.
- **No Remote Identity Servers.** Tranche Two relies exclusively on locally injected tokens (`[PROPOSAL]` e.g., JWT overrides or local Vault tokens); it does not build the remote authorization broker.
- **No Text Depreciation.** `m.Mode == "text"` must continue to function exactly as it did in Tranche One. Human operators remain isolated.

## 4. Proposed Covered-Command Expansion Order
*See `TRANCHE_TWO_COVERED_SET_EXPANSION.md` for full justification and branch mapping.*
Expansion targets deterministic evaluation paths: `node deploy --dry-run`, `spreader package`, and `secrets get`.

## 5. Tranche-Two Launch Preconditions
- Tranche-one acceptance pack is reviewed and approved.
- Tranche-one covered set remains frozen and passing in CI.
- JSON success/failure behavior remains stable under live execution.
- Subprocess capture is introduced and proven on a single bounded command first.
- Uncovered commands strictly bypass all capability enforcement.

## 6. Tranche-Two Completion Conditions
The deployment is complete exclusively when:
1. **Denial Formatting:** Unauthorized requests made against the Tranche Two Covered Set deterministically reject the launch via a strictly formatted JSON `CAPABILITY_DENIED` envelope.
2. **Acceptance Processing:** Authorized requests map to their underlying execution paths normally.
3. **Dry-Run Downgrades:** Executing a flagged mutator (`node deploy --dry-run`) evaluates against `simulate` rights rather than `deploy` capabilities.
4. **Panic Trapping:** A synthetically induced Golang panic inside a covered node is caught, redacted, and emitted structurally via `output.OutputEnvelope` rather than leaking `os.Stderr` stack traces.
5. **Subprocess Wrap:** Targeted commands generating arbitrary child strings cleanly attach their raw output specifically to `OutputEnvelope.Error.SubProcessLog`.
6. **Graceful Degradation:** Legacy (uncovered) commands execute properly, bypassing the new capability model entirely.
