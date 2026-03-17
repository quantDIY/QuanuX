# Phase 2C: Operational Hardening Control Checklist

**Objective:** Make `quanuxctl infra gcp-sql` operationally strict, predictable, auditable, and safe for repeated operator use without widening the approved SQL surface.

## 1. Credential and Runtime Resolution
*   Define one canonical credential resolution order (`OS Keyring` -> `GOOGLE_APPLICATION_CREDENTIALS` env).
*   Define one canonical project resolution order (`OS Keyring` -> `GCP_PROJECT_ID` env).
*   Reject execution if required runtime inputs are missing.
*   Emit deterministic errors for missing/invalid credentials or project identifiers.

**Acceptance Criteria**
*   Given no credentials, `execute` fails closed with exit code 2 and a stable JSON/human shape.
*   Given invalid credentials, `execute` fails closed with exit code 2.
*   Given valid credentials and complete runtime config, `execute` resolves the target without ambiguity.
*   Tests cover every supported resolution path and every missing-input path.

## 2. CLI Output Contract
*   Freeze the stdout/stderr contract for `validate`, `transpile`, and `execute`.
*   Establish exact requirements for human-readable (default) and machine-readable (`--json`) modes.
*   Ensure `TranspilationError` and `RuntimeError` payloads are structurally identical across commands.

**Acceptance Criteria**
*   **JSON Success Mode:** `{"mode": "str", "status": "success", "query_fingerprint": "hash", "rule_surface_version": "tract2_phase1", "bounds": {"max_rows": int, "timeout": int}, "row_count": int, "sql": "str"}`
*   **JSON Rejection Mode:** `{"mode": "str", "status": "error", "error_type": "TranspilationError", "rejected_construct": "str", "violated_rule": "str", "fallback_instruction": "str", "query_fingerprint": "hash"}`
*   `validate` explicitly returns approved/rejected status and the exact rule surface used.
*   Snapshot tests lock the exact format to prevent output drift.

## 3. Bounded Execution Controls
*   Make `--timeout` explicit and enforced.
*   Make `--max-rows` explicit and enforced.
*   Preserve `--dry-run`.
*   Ensure the command **never** silently falls back to native BigQuery execution for rejected transpilation.

**Acceptance Criteria**
*   `execute` with default bounds uses documented defaults (30s, 100 rows) and reports them in the output.
*   Invalid or negative bound values fail *before* query execution with exit code 3.
*   `dry-run` performs validation/transpilation only and does not trigger the GCP target.
*   A rejected query inherently never executes against BigQuery.

## 4. Auditability and Traceability
*   Add a stable query fingerprint (SHA256 hash of the normalized query) to all modes.
*   Explicitly record if the query was merely validated, transpiled, or physically executed.
*   Record rejection reasons if blocked.

**Acceptance Criteria**
*   Success output contains the `query_fingerprint`.
*   Execution output contains explicitly applied bounds and the execution `mode`.
*   Rejection output contains the specific `rejected_construct` and `violated_rule`.
*   JSON mode is directly parseable by downstream pipelines without regex extraction.

## 5. Error Taxonomy
*   Map failure classes to a deterministic exit code policy:
    *   `0`: Success
    *   `1`: `TranspilationError` (Validation/Rejection of Matrix bounds)
    *   `2`: `AuthError` / `ConfigError` (Missing credentials/project)
    *   `3`: `RuntimeError` (Invalid inputs, invalid bounds)
    *   `4`: `ExecutionError` (BigQuery remote failure)

**Acceptance Criteria**
*   The same error class always produces the exact deterministic exit code family.
*   `TranspilationError` always exits `1` and strictly avoids leaking a Python traceback in standard runs.
*   Internal/Unexpected exceptions are labelled specifically as internal pipeline failures.

## 6. Operator UX Alignment
*   Ensure CLI output headers mirror the specific Fail-Closed wording from `QuanuX-Annex/README.md`.

**Acceptance Criteria**
*   CLI rejection strings match the published unsupported-construct language exactly.
*   Documentation accurately simulates real command outputs for missing credentials and banned joins.

## 7. Test Hardening
*   Add snapshot or golden-file tests for stable input/output formats.
*   Cover `validate`, `transpile`, `execute`, and `dry-run` modes.

**Acceptance Criteria**
*   Tests cover human-readable vs machine-readable output flags.
*   Tests assert non-zero exit codes structure when given missing credentials, banned joins, or unsupported CTEs.
*   Tests enforce that output configurations do not organically drift.

## 8. Scope Discipline
*   **Do not** add new SQL features to the transpiler.
*   Internal optimizer artifacts (`TOP_N`, `STREAMING_LIMIT`, `rowid` semi-join) remain explicit compiler exemptions, not operator-facing features.

**Acceptance Criteria**
*   User-facing approved SQL surface remains identical to Phase 1.
