# QuanuX GCP Integration: Tract 2 Control Specification

**Target Audience:** Red Team, Data Engineers, Research Tier
**Status:** Prototype Quarantine (Pending Red Team Promotion)
**Scope:** DuckDB-to-BigQuery AST Transpilation & Query Adapter

## 1. Objective and Boundary
Tract 2 focuses on bridging the analytical query layers. The objective is to build a transpilation prototype that adapts local DuckDB SQL queries into BigQuery Standard SQL, enabling researchers to seamlessly query the GCP Historical Lake established in Tract 1. 

**Mandate:** This layer exists strictly as a research convenience prototype. It must explicitly quarantine the AST transpiler from the foundational Tract 1 ingestion pipeline and any Tier 4 paths.

## 2. Approved Query Surface (Phase 1 Locked Matrix)
The transpiler is approved to handle a narrow, explicitly whitelisted subset of SQL essential for quantitative research against the `MarketTick` schema. Any function or clause not on this exact whitelist will trigger a fail-closed rejection.

**Approved SQL surface**
* `SELECT`
* `FROM`
* `WHERE`
* `GROUP BY`
* `ORDER BY`
* `LIMIT`

**Approved aggregates**
* `COUNT`
* `SUM`
* `AVG` (Note: Semantic parity tests for averages must define an explicit floating-point tolerance boundary; all other aggregates require exact matches).
* `MIN`
* `MAX`

**Allowed basics**
* explicit column aliases
* simple numeric and string literals
* straightforward comparisons and boolean predicates used inside `WHERE`

**Boundary Enforcement:** Direct BigQuery access must be preserved. Any query exceeding the transpiler's approved subset should be executed directly against BigQuery via the native client, bypassing the DuckDB compatibility layer entirely. Note: `TOP_N` is strictly an internal DuckDB AST node map for parsing `LIMIT` and is **not** an approved user-facing query construct.

## 3. Unsupported SQL Features
The transpilation prototype will **not** support or attempt to translate complex or dialect-specific features to prevent dangerous or wildly inefficient remote execution. The following are explicitly unauthorized:
*   joins
*   window functions
*   CTEs (Common Table Expressions)
*   subqueries beyond the exact cases already parity-tested
*   proprietary DuckDB macros/functions
*   mutation statements of any kind (`DROP`, `ALTER`, `UPDATE`, `INSERT`, `DELETE`)

## 4. Fallback Behavior
The transpiler must implement a strict **Fail-Closed Fallback** policy:
*   If the AST parser encounters an unsupported token, syntax, or structural discrepancy, it must **halt immediately** and raise an explicit `TranspilationError`.
*   Silent translation degradation or heuristic "best effort" translations are strictly forbidden.
*   **Visibility and Determinism:** On failure, the `TranspilationError` message must be fully deterministic and explicitly declare:
    1.  The specific unsupported construct that was detected.
    2.  The reason it was rejected and disqualified from the whitelist.
    3.  A direct instruction to the operator/researcher outlining the required fallback path (e.g., simplifying the query or triggering native BigQuery direct mode).

## 5. Prototype Promotion Criteria
To graduate from Tract 2 Quarantine into the Approved Architecture, the transpiler must pass a formal Red Team audit against the following proof criteria:
1.  **Parsing Exactness:** Must achieve 100% deterministic success against a defined target suite of authorized DuckDB queries.
2.  **Explicit Semantic Parity:** "Dataset parity" is operationally defined as mathematically perfect alignment. Executing an authorized query locally (DuckDB) and remotely (transpiled BigQuery) must result in:
    *   The exact same row set.
    *   The exact same grouping cardinality.
    *   Identical numeric outputs within an explicit standard floating-point tolerance boundary.
    *   Identical null-handling and empty-set behavior.
3.  **Result Set Bounding:** Must prove bounded memory limits for retrieving BigQuery result sets back into the Python/Arrow research tier (chunked retrieval).
4.  **No Unauthorized Execution:** Must prove it physically cannot invoke remote BigQuery execution commands that manipulate table state (`DROP`, `ALTER`, `UPDATE`, `INSERT`). The transpiler must enforce a strict read-only execution matrix.
