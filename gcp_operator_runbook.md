# QuanuX Tract 2: GCP SQL Operator Runbook

## Overview
The `quanuxctl infra gcp-sql` command surface allows authorized QuanuX operators to seamlessly query remote BigQuery datasets utilizing native DuckDB AST query dialects. 
**This execution surface is Fail-Closed.** Any SQL query outside the strict Phase 1 Query Matrix will immediately fail before network execution.

### Approved SQL surface

*   `SELECT`, `FROM`, `WHERE`, `GROUP BY`, `ORDER BY`, `LIMIT`
*   Allowed Aggregates: `COUNT`, `SUM`, `AVG`, `MIN`, `MAX`
*   **Bounded Subqueries:**
    *   Scalar Subqueries in `SELECT` (single row/column guarantee).
    *   `WHERE` Filter Subqueries restricted strictly to explicit exact memberships:
        *   `expr IN (SELECT single_column ...)`
        *   Scalar comparison forms that return exactly one row and one column.
    *   Uncorrelated Derived Tables in `FROM` clauses.
    *   **Nesting depth is strictly capped at one level.** No correlated subqueries, no nested mutations, and no deep chains.

### Unsupported constructs (Fail-Closed Matrix)
All explicit mutations, cross joins, internal joins (`INNER`, `LEFT`, `RIGHT`), Window functions, recursive CTEs.

## 1. Prerequisites (Credentials & Runtime Setup)

Tract 2 execution requires an active BigQuery Project ID and a Google Cloud Service Account JSON Key properly mapped into the host ecosystem either via the OS Keyring (Zero-Disk) or Environment variables.

**(Preferred) Zero-Disk Keyring Configuration**
```bash
quanuxctl secrets set GCP_PROJECT_ID
quanuxctl secrets set GOOGLE_APPLICATION_CREDENTIALS
```

*(Fallback) Shell Environment Setup*
```bash
export GCP_PROJECT_ID="your-gcp-project-123"
export GOOGLE_APPLICATION_CREDENTIALS="/path/to/key.json"
```

## 2. Command Flow: Validate, Transpile, Execute

You can interact with the engine using three explicit states:

### Validate Only
Checks if your query conforms strictly to the approved Phase 1 bounds (no network calls).
```bash
quanuxctl infra gcp-sql validate "SELECT level FROM MarketTick LIMIT 10"
```
**Success Returns (Exit 0):** `SUCCESS: Query is within the approved Phase 1 bounded matrix.`

### Transpile Only
Emits the exact BigQuery Standard SQL translation without executing.
```bash
quanuxctl infra gcp-sql transpile "SELECT level, AVG(bid_price) FROM MarketTick GROUP BY level LIMIT 1"
```
```sql
# Output
SELECT level, AVG(bid_price) FROM MarketTick GROUP BY level LIMIT 1
```

### Full Execution
Requests full execution against the attached GCP `Client` with mandated fallback limits.
```bash
quanuxctl infra gcp-sql execute "SELECT instrument_id, COUNT(*) FROM MarketTick GROUP BY instrument_id" --timeout 30 --max-rows 100
```
**Example Bounds:**
*   `--timeout 30`: Disconnect natively if the query takes longer than 30s.
*   `--max-rows 100`: Truncate the PyArrow iterable chunk downloads dynamically to prevent memory blowout locally.
*   `--dry-run`: Transpile and format execution plans without executing.

## 3. Dealing With `TranspilationError`

The Tract 2 engine strictly outright bans complex features to prevent data leakage and excessive compute overages. **There is no silent translation fallback.** 

If you attempt a `JOIN`, `CTE`, `WINDOW FUNCTION` or state-mutating request (`UPDATE`, `DROP`), you will encounter a fatal non-zero exit prompt.

```bash
quanuxctl infra gcp-sql execute "SELECT AVG(bid) OVER(PARTITION BY level) FROM MarketTick"
```
**Output (Exit 1):**
```
FATAL: Prototype Matrix Boundary Violation
Rejected Construct: WindowFunction
Violated Rule: Window functions are explicitly banned under the Tract 2 Control Spec

Fallback required: Please execute complex aggregations natively via the BigQuery client.
```
**Resolution:** Proceed direct to the BigQuery UI or Python BQ Client library to generate the complex metrics manually.

## 4. Machine-Readable Execution (`--json`)

If wrapping the CLI inside larger DAG-based engines, enforce `--json`. Output will structure immediately for CI logic:
```json
{
  "mode": "execute", 
  "status": "success", 
  "query_fingerprint": "8c502b...b439e", 
  "rule_surface_version": "tract2_phase1", 
  "bounds": {"max_rows": 100, "timeout": 30}, 
  "row_count": 50, 
  "sql": "SELECT instrument_id FROM MarketTick LIMIT 50"
}
```

### Error Taxonomy
JSON engines must trap these specific exit codes correctly:
*   `Exit 0`: Query Success.
*   `Exit 1`: Validation `TranspilationError` (Target violated allowed Tract 2 matrix).
*   `Exit 2`: `ConfigError` or `AuthError` (Missing project or keys).
*   `Exit 3`: `RuntimeError` (Invalid input bounds e.g. `-5` max rows).
*   `Exit 4`: `ExecutionError` (Valid GCP credentials and Matrix, but remote client execution timed out/failed).
