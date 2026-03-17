# QuanuX-Annex BigQuery Transpiler

This module validates and transpiles DuckDB compatible SQL queries into precise, bounded BigQuery execution patterns by intercepting the DuckDB IR AST representation.

## Phase 1 Analytical Extension
Recent enhancements to Phase 1 allowed an extension of the explicit JOIN matrix for the warm analytical tier. The transpiler acts as an explicit firewall against unbounded Cartesian explosion.

### Enforced Join Rules
The transpiler enforce strict structural limits on joining datasets within BigQuery:
1. **Time-Series Correlation**: `ASOF JOIN` and windowed `INNER JOIN` operations mapped exclusively on `instrument_id` and `timestamp_ns` are structurally checked and allowed.
2. **Reference Enrichment**: `LEFT JOIN` operations targeting static, whitelisted reference datasets (e.g., `symbol_mapping`, `venue_dim`) are allowed.
3. **Explosive Operations Banned**: `CROSS JOIN`, `FULL OUTER JOIN`, `RIGHT JOIN`, and unbounded comma cross joins are forcefully aborted and synthesize a Transpilation Error.

For more information on expanding AST rules, read `SKILL.md`.
