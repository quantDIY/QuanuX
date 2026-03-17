# QuanuX Phase 3B: Bounded Joins Control Spec

## 1. Intent and Scope
Phase 3B introduces the most delicate authorized expansion to the `gcp-sql` baseline: **Controlled Joins**. To prevent Cartesian explosion, catastrophic remote compute overruns, or complex nested logic masking data leakage, the surface is ruthlessly restricted to atomic, exact-match relational bridging.

## 2. Approved Join Surface (The "Allowed" Matrix)
Code will fail-closed if it deviates from this singular accepted structure:

*   **Single Join Only:** A query may contain at most one `JOIN` operation.
*   **`INNER JOIN` Only:** Only explicit `INNER JOIN` (or default `JOIN` assuming `INNER`) is permitted.
*   **Equality Predicates Only:** The `ON` clause must consist of a strict equality check between explicit column references (e.g., `ON a.instrument_id = b.instrument_id`).
*   **No Join Chains:** A single bridging of exactly two referenced tables (or self-aliases) is the absolute ceiling.

## 3. Explicitly Banned Join Behaviors (The "Rejected" Matrix)
The following constructs are strictly banned and must issue a deterministic `TranspilationError`:

*   **Outer & Cross Joins:** `LEFT`, `RIGHT`, `FULL`, `OUTER`, and `CROSS` (user-facing) joins.
*   **Multiple Joins:** Any attempt to chain joins (e.g., `A JOIN B JOIN C`).
*   **Non-Equality Predicates:** Range joins (`>`), inequality (`!=`), or logic incorporating expressions/functions inside the `ON` clause.
*   **Implicit Relational Bridges:** `NATURAL` joins, `USING` clauses, or comma-separated `FROM A, B` implicit cross joins.
*   **Mixed Complexity:** No joins inside derived tables/subqueries, and no joins combined with subqueries or aggregations until independently proven safe.
*   **Correlated Equivalents:** Emulating join-like behavior via correlated subqueries remains comprehensively banned under the Phase 3A specs.

## 4. Anti-Drift Inheritance Rule
**Internal DuckDB artifacts previously accepted for bounded subquery execution — including `CROSS_PRODUCT`, internal `HASH_JOIN`, and internal `first()` lowering — do not constitute user-facing join authorization and may not be cited as evidence of Phase 3B support.**

## 5. Promotion Criteria to `main` (Phase 3B Acceptance)
1.  **Strict Transpiler Isolation:** `test_internal_subquery_artifacts_explicit` and all previous tests must remain fully passing without interference.
2.  **Corpus Validation:** Both allowed (single inner equality) and rejected (multi-join, outer join, expression predicates) scenarios must be tracked as static `.sql` fixtures.
3.  **Live BigQuery Parity Proof:** Execution must run over the actual network validating pyarrow schema translation for merged tabular output.
4.  **No CLI Drift:** All UI boundary outputs inside `test_gcp_cli_golden.py` remain pristine.
