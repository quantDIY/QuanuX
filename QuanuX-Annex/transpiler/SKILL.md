---
name: Transpiler Hardening & Execution
description: Guidelines for adding functions to the AST translation map for QuanuX-Annex duckdb transpiler
---

# Transpiler Management

The transpiler operates logically off the DuckDB JSON EXPLAIN IR plan. To ensure deterministic bounding, **everything is fail-closed by default**.

## Workflow to Expand the Translation Matrix

To add new constructs (like novel join nodes, aggregates, or table whitelist allowances):

1. **Verify Target IR Representation**: Run DuckDB's `EXPLAIN (FORMAT JSON) <query>` to see exactly how DuckDB interprets the new target constraint and prints it in the logical planner tree.
2. **Update Check Boundaries**: Modify `QuanuX-Annex/transpiler/gcp_transpiler.py`.
    - Modify the `allowed_nodes` set if a new IR token appears (like `WINDOW` or `ASOF_JOIN`).
    - Expand explicit logic in `_traverse_relational_node` rather than string-matching `query`. The string matching inside `_enforce_read_only` or `_enforce_join_rules` acts merely as a fast-fail defense line before full parse compilation.
3. **Extend Whitelisted Datasets**: If expanding reference enrichment domains for `LEFT JOIN`, update the nested `whitelist = ["symbol_mapping", "venue_dim", ...]` in `_traverse_relational_node`.
4. **Unit Validation**: Add tests to `test_gcp_transpiler.py` representing both the exact allowance syntax parity as well as explicit evasive/banned attempts.
