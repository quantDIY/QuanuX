# QuanuX GCP Integration: Tract 2 Control Specification

**Target Audience:** Red Team, Data Engineers, Research Tier
**Status:** Prototype Quarantine (Pending Red Team Promotion)
**Scope:** DuckDB-to-BigQuery AST Transpilation & Query Adapter

## 1. Objective and Boundary
Tract 2 focuses on bridging the analytical query layers. The objective is to build a transpilation prototype that adapts local DuckDB SQL queries into BigQuery Standard SQL, enabling researchers to seamlessly query the GCP Historical Lake established in Tract 1. 

**Mandate:** This layer exists strictly as a research convenience prototype. It must explicitly quarantine the AST transpiler from the foundational Tract 1 ingestion pipeline and any Tier 4 paths.

## 2. Approved Query Surface
The transpiler is approved to handle a narrow, clearly defined subset of SQL essential for quantitative research against the `MarketTick` schema:
*   Standard `SELECT`, `FROM`, `WHERE` clauses.
*   Basic aggregations (`SUM`, `AVG`, `MIN`, `MAX`, `COUNT`).
*   Time-series bucketing / basic `GROUP BY` logic.
*   Simple `JOIN` conditions assuming standard schemas.

**Boundary Enforcement:** Direct BigQuery access must be preserved. Any query exceeding the transpiler's approved subset should be executed directly against BigQuery via the native client, bypassing the DuckDB compatibility layer entirely.

## 3. Unsupported SQL Features
The transpilation prototype will **not** support or attempt to translate complex or dialect-specific features to prevent dangerous or wildly inefficient remote execution:
*   Complex recursive Common Table Expressions (CTEs).
*   Deeply nested or complex Window Functions.
*   DuckDB-specific extensions, pragmas, or proprietary macros.
*   Cross-cloud joins or federated queries outside the bound GCP datasets.

## 4. Fallback Behavior
The transpiler must implement a strict **Fail-Closed Fallback** policy:
*   If the AST parser encounters an unsupported token, syntax, or structural discrepancy, it must **halt immediately** and raise an explicit `TranspilationError`.
*   Silent translation degradation or heuristic "best effort" translations are forbidden.
*   On failure, the error message must instruct the operator/researcher to either simplify the DuckDB query to the supported subset or utilize the direct native BigQuery client.

## 5. Prototype Promotion Criteria
To graduate from Tract 2 Quarantine into the Approved Architecture, the transpiler must pass a formal Red Team audit against the following proof criteria:
1.  **Parsing Exactness:** Must achieve 100% deterministic success against a defined target suite of authorized DuckDB queries.
2.  **Dataset Parity:** Must mathematically prove that executing a supported query locally in DuckDB and executing the transpiled query remotely in BigQuery returns identical resultant datasets.
3.  **Result Set Bounding:** Must prove bounded memory limits for retrieving BigQuery result sets back into the Python/Arrow research tier (chunked retrieval).
4.  **No Unauthorized Execution:** Must prove it physically cannot invoke remote BigQuery execution commands that manipulate table state (`DROP`, `ALTER`, `UPDATE`, `INSERT`). The transpiler must enforce a strict read-only execution matrix.
