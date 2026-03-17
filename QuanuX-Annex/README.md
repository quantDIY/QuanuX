# QuanuX-Annex Data Lake Engine

The `QuanuX-Annex` is a Tier-1 high-frequency data ingestion and retrieval engine. It operates natively in C++ to achieve zero-allocation processing of NATS JetStream telemetry directly into DigitalOcean Spaces (Zarr Cloud Drop). It concurrently mounts a GraphQL HTTP Webhook listener (`cpp-httplib`) seamlessly hooking into the Hasura Federation to stream historical market analytics back to the centralized execution matrix.

## Core Capabilities
- **Zero-Allocation NATS Subscriber**: Binds natively to `QUANUX.MARKET.TICK`, mapping binary payloads straight into aligned C-structs in RAM.
- **AWS SigV4 Cloud Drops**: Custom cryptographic authentication streams compressed Blosc2 arrays directly into DO Spaces via `curl_multi` without executing expensive SHA256 payload hashing (`UNSIGNED-PAYLOAD`).
- **Federated Egress Matrix**: Streams historical market ticks direct from the DO Spaces blob boundary into compliant JSON arrays for Hasura Remote Schemas. Egress threads natively leverage chunked S3 streaming over `snprintf` to prevent OOM RAM inflation on massive wide-time-window queries.

## Zero-to-Production Deployments
Deployments to the DigitalOcean Edge Nodes are heavily automated via `quanuxctl`. Ephemeral authentication directly extracts from the native MacOS OS Keychain, circumventing `.env` text file hazards.

Follow the complete operations guide at:
[`docs/Zero-to-Production.md`](docs/Zero-to-Production.md)

### Edge Node Command Interfaces
The `quanuxctl` CLI provides infrastructure control over the QuanuX nodes:
```bash
# Verify active deployment droplets:
quanuxctl infra do-droplets

# Verify active Data Lake boundaries:
quanuxctl infra do-spaces
```
## Tract 2: Research Database Transpiler
The QuanuX-Annex includes the `QuanuXDuckToBQTranspiler`, an execution layer designed to bridge local DuckDB queries into BigQuery Standard SQL text for bounded remote execution.

To guarantee zero unauthorized mutation and maintain strict dataset parity, the transpiler operates under a mathematically verified Approved Query Matrix:
- **Approved SQL Surface:** `SELECT`, `FROM`, `WHERE`, `GROUP BY`, `ORDER BY`, `LIMIT`.
- **Approved Aggregates:** `COUNT`, `SUM`, `AVG`, `MIN`, `MAX`.
- **Allowed Basics:** Explicit column aliases, numeric/string literals, and basic boolean predicates.
- **Bounded Subqueries (Phase 3A):** Scalar subqueries in `SELECT`, simple `IN (SELECT...)` filters in `WHERE`, Uncorrelated Derived Tables in `FROM`. Maximum nesting depth 1.
- **Bounded Joins (Phase 3B):** Strictly one `INNER JOIN` (or self-join) via explicit column equality predicates.

**Unsupported Constructs (Fail-Closed):**
The transpiler enforces physical read-only limits by strictly blocking state-mutating commands (`DROP`, `ALTER`, `UPDATE`, `INSERT`, `DELETE`). Due to complex dialect variance, it explicitly rejects advanced routing syntax such as:
- Outer, Cross, Natural, and Multiple Joins
- Joins mixed with Subqueries or Aggregations
- Window Functions
- Common Table Expressions (CTEs)
- Correlated Subqueries and recursive CTEs
- DuckDB proprietary macros/functions

> **Completion Status**: Tract 2 is complete as a bounded, operator-ready DuckDB→BigQuery transpiler track under the currently approved matrix. Complete does not mean broad SQL compatibility; it means the current approved surface is credible, tested, operatorized, and documented enough to freeze.

Any query exceeding this whitelist will natively raise a `TranspilationError` and halt immediately before querying GCP. Operators must execute unauthorized complex logic natively against BigQuery if bypassing this prototype boundary.

### Operator Rejection Examples
When researchers attempt queries outside the bounded surface, expect explicit, deterministic `TranspilationError` stack traces indicating the exact failure reason:

**Example 1: Banned Window Functions**
```sql
SELECT AVG(bid_price) OVER(PARTITION BY level) FROM MarketTick
```
> `gcp_transpiler.TranspilationError: Unsupported construct: WindowFunction...`

**Example 2: Banned Outer/Multiple Joins**
```sql
SELECT a.level FROM MarketTick a LEFT JOIN MarketTick b ON a.level = b.level
```
> `gcp_transpiler.TranspilationError: Outer, Cross, and Natural joins are strictly banned under Phase 3B...`

## Agent Tools & Autonomous Systems
Agent AI architecture contexts have been directly injected into every module via `SKILL.md` documents. Ensure parsing of `src/resolvers/SKILL.md` and `src/federation/SKILL.md` before initiating memory operations.
