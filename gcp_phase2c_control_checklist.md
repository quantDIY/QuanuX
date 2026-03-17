# Phase 2C: Operational Hardening Control Checklist

**Objective:** Make `quanuxctl infra gcp-sql` operationally strict, predictable, auditable, and safe for repeated operator use without widening the approved SQL surface.

## 1. Credential and Runtime Resolution
*   Define one canonical credential resolution order (e.g., `OS Keyring` -> `os.environ`).
*   Define one canonical project resolution order.
*   Define one canonical dataset/table resolution order.
*   Reject execution if required runtime inputs are missing.
*   Emit deterministic, operator-readable errors for:
    *   missing credentials
    *   invalid credentials
    *   missing project
    *   missing dataset/table
    *   BigQuery client initialization failure

## 2. CLI Output Contract
*   Freeze the stdout/stderr contract for `validate`, `transpile`, and `execute`.
*   Define one machine-readable mode (e.g., `--json`).
*   Define one human-readable mode (default).
*   Ensure `TranspilationError` output is structurally identical across all commands.
*   Ensure runtime/auth/BigQuery failures have their own deterministic output class.

## 3. Bounded Execution Controls
*   Make `--timeout` explicit and enforced (reject negative or zero values).
*   Make `--max-rows` explicit and enforced (reject negative or zero values).
*   Preserve `--dry-run`.
*   Ensure the command never silently falls back to native BigQuery execution for rejected transpilation.

## 4. Auditability and Traceability
*   Add a stable query fingerprint (e.g., SHA256 hash of the normalized query).
*   Record whether the query was only validated, transpiled, or executed.
*   Record the bounds applied.
*   Record rejection reason when blocked.

## 5. Error Taxonomy
*   Separate errors into stable classes:
    *   validation/rejection
    *   credential/auth
    *   configuration/runtime
    *   BigQuery execution
    *   unexpected internal error
*   Map each class to a deterministic exit code policy (e.g., `1` for validation, `2` for auth).

## 6. Operator UX Alignment
*   Ensure CLI messages mirror `QuanuX-Annex/README.md` operator documentation language exactly.

## 7. Test Hardening
*   Add snapshot or golden-file tests for stable output formats (human and machine-readable).
*   Cover all three command modes (`validate`, `transpile`, `execute`).
*   Test invalid bounds, missing credentials, timeout handling, and specific rejections (JOIN, CTE, WINDOW).

## 8. Scope Discipline
*   **Do not** add new SQL features.
*   Internal DuckDB optimizer artifacts (`TOP_N`, `STREAMING_LIMIT`, `rowid` semi-join) remain explicit and internal-only, with narrow explanations and tests.
