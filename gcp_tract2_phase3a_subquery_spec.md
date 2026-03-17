# QuanuX Phase 3A: Bounded Subquery Control Spec

## 1. Intent and Scope
As part of Phase 3, the `gcp-sql` transpiler will safely widen its supported SQL surface to include **Bounded Subqueries**. The intent is to support common analytical access patterns required by researchers, without introducing the unbounded complexity or performance degradation associated with arbitrary nested execution graphs or implicit cross-joins.

## 2. Approved Subquery Surface (The "Allowed" Matrix)

The transpiler will explicitly whitelist the following DuckDB Logical/Physical AST pattern representations of subqueries:

*   **Scalar Subqueries in `SELECT` lists:** Subqueries that guarantee a single column, single row return value.
    *   *Example:* `SELECT instrument_id, (SELECT MAX(bid_price) FROM MarketTick) as global_max, bid_size FROM MarketTick`
*   **Scalar Subqueries in `WHERE` predicates (Filter):** Explicit single-column `IN` membership (`expr IN (SELECT single_column ...)`) or strict scalar comparisons that return exactly one row and one column.
    *   *Example:* `SELECT instrument_id FROM MarketTick WHERE level IN (SELECT level FROM MarketTick WHERE bid_price > 100)`
*   **FROM Clause Subqueries (Uncorrelated Derived Tables):** Used exclusively for structural grouping prior to a top-level limit or filter.
    *   *Example:* `SELECT t.instrument_id, t.bid_price FROM (SELECT instrument_id, bid_price FROM MarketTick WHERE level = 1) t ORDER BY t.bid_price`

## 3. Explicitly Banned Subquery Behaviors (The "Rejected" Matrix)

The fail-closed policy remains absolute. The following extensions of subqueries must raise an immediate `TranspilationError`:

*   **Correlated Subqueries:** Any subquery whose inner predicate references a column from the outer query block. This prevents N+1 execution scaling in remote compute.
*   **Nested Subquery Depth > 1:** The matrix explicitly caps subquery depth at a single level. Subquery-inside-subquery chains or mixed derived-table plus nested scalar-subquery combinations are strictly banned.
*   **Nested Mutations:** Any subquery attempting to project state changes (e.g. `RETURNING` mutations or hidden CTE updates).
*   **Lateral Joins:** No implicit or explicit `LATERAL` unnesting that executes per row.
*   **Complex Subqueries inside Aggregates:** `SUM( (SELECT...) )` will remain blocked until semantic parity of deep nested limits is proven.

## 4. Promotion Criteria to `main` (Phase 3A Acceptance)

Code will not be merged into the Tract 2 core unless all four of the following criteria are met:

1.  **Allowed/Rejected Corpus Update:** The `fixtures/gcp_sql/allowed` directory must contain passing tests for scalar, list, and simple FROM subqueries. The `rejected` directory must include explicit examples of correlated subqueries that fail transpilation deterministically.
2.  **Live BigQuery Parity Proof:** Execution of the allowed subquery corpus must be verified against actual BigQuery datasets (via `test_gcp_transpiler.py`), ensuring float precision logic and grouping parity holds firm across the network translation.
3.  **No Ast Artifact Bleeding:** Support for subqueries must not accidentally whitelist broader `JOIN` statements or `WINDOW` nodes.
4.  **CLI Output Stability:** The human-readable outputs and JSON schema shapes established in Phase 2D must remain frozen and natively surface any nested AST rejections gracefully.

## 5. Phase 3B Inheritance Cap
Internal DuckDB optimizer or planner artifacts accepted for bounded subquery execution (`CROSS_PRODUCT`, internal `HASH_JOIN`, internal `first()`) do not constitute user-facing join authorization and cannot be cited as prior proof for Phase 3B.
