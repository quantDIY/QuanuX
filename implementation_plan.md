# QuanuX GCP Integration: Tract 2 Prototype Implementation Plan

## Goal Description
Develop the DuckDB SQL to BigQuery Standard SQL transpilation prototype for the Research Tier. This pipeline acts as a convenience layer, allowing standard local quantitative queries on the `MarketTick` schema to be seamlessly routed to the BigQuery external tables established in Tract 1.

## User Review Required
> [!IMPORTANT]
> Awaiting Red Team sign-off on this revised **Tract 2 Implementation Plan** before beginning execution. All 6 constraints from the Control Specification review have been functionally mapped to the proposed Python architecture below.

## Proposed Changes

### Core Transpiler Architect (`QuanuX-Annex/gcp_transpiler.py`)
This module provides the deterministic bridge from local research DuckDB syntax to remote BigQuery execution.

#### [NEW] gcp_transpiler.py

*   **`TranspilationError(Exception)`**:
    *   A custom exception class serving as the Fail-Closed mechanism.
    *   **Fallback Behavior**: When raised, the error message will deterministically output: 
        1. The specific unsupported AST node/token (e.g., `Unsupported construct: WindowFunction`).
        2. The rejection reason (`Window functions are explicitly banned under the Tract 2 Control Spec`).
        3. The required operator fallback path (`Fallback required: Please execute complex aggregations natively via the BigQuery client`).

*   **Translation Boundary (DuckDB Substrait)**:
    *   The transpiler will consume raw DuckDB SQL strings.
    *   Instead of writing custom regex or a fragile string parser, it will lean on DuckDB's native parser by executing `conn.get_substrait(query)` to extract the canonical Intermediate Representation (IR).
    *   *Note on Overclaiming:* Validation of the Substrait IR against BigQuery Standard SQL is strictly experimental for this prototype. Promotion to Tract 1 relies entirely on passing the semantic parity test sweeps.

*   **`QuanuXDuckToBQTranspiler` Class**:
    *   **Read-Only Matrix Enforcement**: Before any AST/Substrait parsing begins, the input string will be strictly scanned to ban non-`SELECT` operations (`DROP`, `ALTER`, `UPDATE`, `INSERT`).
    *   **Whitelist Enforcement**: The class will traverse the Substrait relational algebra nodes. It will implement a strictly allowed list (`SELECT`, `ProjectRel`, `AggregateRel`, `FilterRel`). If an unrecognized Relational Node, mathematical operation, or unapproved function (like Windowing or recursive CTE mapping) is detected, it instantly fires `TranspilationError`.
    *   **Result Set Bounding**: The class will output not just the SQL string, but a controlled BigQuery execution block utilizing `query_job.result().to_arrow_iterable()` to guarantee chunked, memory-safe data retrieval back to the Python tier.

### Pytest Coverage (`tests/test_gcp_transpiler.py`)
The testing methodology abandons the Tract 1 ingestion shape in favor of strict parser and semantic parity assertions.

#### [NEW] test_gcp_transpiler.py

*   **Whitelist Acceptance Tests**: Asserts that `SELECT`, `FROM`, `WHERE`, `GROUP BY`, and standard aggregations (`SUM`, `AVG`, `MIN`, `MAX`, `COUNT`) map perfectly to BigQuery strings without raising exceptions.
*   **Unsupported Construct Rejection Tests**: Explicitly injects Window Functions, dialect-specific macros, and CTEs to verify that `TranspilationError` is thrown deterministically.
*   **Fallback Message Determinism**: Asserts that the exception `__str__` exactly matches the required 3-part fallback structure demanded by the Control Spec.
*   **Semantic Parity Fixtures**: (Core Graduation Requirement) Executes the transpiled approved queries against a mocked/simulated layout and asserts exact row-count, grouping cardinality, explicit null-handling, and numeric precision against local DuckDB results.
*   **State-Mutation Bans**: Asserts that sending an `UPDATE` or `DROP TABLE` text to the transpiler triggers an immediate, unrecoverable exception prior to any parsing attempt.

## Verification Plan
1.  **Red Team Review**: Awaiting code-level approval on the transpiler architecture and test shape outlined above.
2.  **Implementation Execution**: Code `gcp_transpiler.py` and `test_gcp_transpiler.py` strictly against this class structure.
3.  **Audit PyTest Runner**: Output testing evidence to `tract2_test_run.log` and push for final promotion evaluation.
