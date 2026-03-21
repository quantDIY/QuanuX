# Tranche Two Covered Set Expansion

This document proposes the next minimum viable expansion slice for the `qxctl` AI Covered Set.

## 1. Current Tranche One Base
- `vault status` (Inspect loop)
- `query validate` (Validate loop)

## 2. Tranche Two Expansion Proposal
The Covered Set will systematically absorb exactly three new endpoints specifically chosen to validate complex boundary behaviors without deploying execution risks:

1. **`secrets get`**
   - *Why:* Safe, pure-data read operation. Validates that string payloads encode accurately into `OutputEnvelope.Data` without corruption. Currently proven as a safe stub returning `nil`.
2. **`spreader package`**
   - *Why:* Local string-bound building execution loop. Evaluates `os/exec` buffering without mutating distributed live environments. Validates `SubProcessLog` buffers.
3. **`node deploy --dry-run`**
   - *Why:* Unlocks the authorization downgrade test. Validates that submitting the `--dry-run` flag visibly lowers the capability firewall mathematically from `deploy` strictly to `simulate`.

## 3. Later Candidates (Deferred)
- **`infra apply --dry-run`**
  - Deferred until dynamic subprocess capture and semantic downgrades are proven securely on the smaller blast-radius targets above.

## 4. Selection Rules
- **Rule 1 (Determinism):** Never absorb commands that stream continuous endless loops (e.g., `telemetry start`).
- **Rule 2 (No Mutating Actioners):** Exclude raw impactors (`secrets set`, `node deploy` without dry-run flags) entirely until Tranche Three. The capability gate must be proven before granting deployment authority.
- **Rule 3 (Branch Truth Matching):** The above endpoints intentionally match paths already proven safe or physically verifiable inside `EVIDENCE_APPENDIX_COMMAND_SURFACE_PROOF.md`.
