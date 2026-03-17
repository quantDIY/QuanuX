import duckdb
import json
import re

class TranspilationError(Exception):
    def __init__(self, construct: str, reason: str):
        self.construct = construct
        self.reason = reason
        self.fallback = "Fallback required: Please execute complex aggregations natively via the BigQuery client."
        super().__init__(self.__str__())
        
    def __str__(self):
        return f"Unsupported construct: {self.construct}. {self.reason}. {self.fallback}"

class QuanuXDuckToBQTranspiler:
    def __init__(self):
        self.conn = duckdb.connect(':memory:')
        
        # We need a schema registry so EXPLAIN actually parses the queries against MarketTick
        self.conn.execute("""
            CREATE TABLE MarketTick (
                timestamp_ns BIGINT, 
                instrument_id UINTEGER, 
                bid_price DOUBLE, 
                ask_price DOUBLE, 
                bid_size UINTEGER, 
                ask_size UINTEGER, 
                level UTINYINT
            );
        """)

    def _enforce_read_only(self, query: str):
        """Scans the query specifically blocking state-mutating prefixes."""
        q = query.strip().upper()
        if not q.startswith("SELECT"):
            if q.startswith("DROP") or q.startswith("ALTER") or q.startswith("UPDATE") or q.startswith("INSERT") or q.startswith("DELETE"):
                raise TranspilationError(q.split()[0], "State-mutating operations are strictly banned prior to AST translation")
            raise TranspilationError(q.split()[0] if q else "EMPTY", "Only SELECT statements are authorized")

    def _enforce_subquery_rules(self, query: str):
        """Enforces limits on nested subqueries prior to IR mapping to prevent parser evasion."""
        # 1. Enforce max subquery depth = 1
        depth = 0
        max_depth = 0
        
        # Tokenize by treating parentheses as explicit boundaries
        tokens = query.replace("(", " ( ").replace(")", " ) ").split()
        in_select_parens = []

        for t in tokens:
            if t == "(":
                in_select_parens.append(False)
            elif t.upper() == "SELECT" and len(in_select_parens) > 0:
                in_select_parens[-1] = True
                depth = sum(in_select_parens)
                # Cap nesting depth at 1 as per Phase 3A Spec
                if depth > 1:
                    raise TranspilationError("NestedSubquery", "Nested Subquery Depth > 1 is strictly banned under Phase 3A Control Spec")
            elif t == ")":
                if len(in_select_parens) > 0:
                    in_select_parens.pop()
                    
        # 2. Heuristically ban complex subqueries inside aggregates. e.g SUM( (SELECT...) )
        q_upper = query.upper()
        if "SUM(" in q_upper or "AVG(" in q_upper or "MIN(" in q_upper or "MAX(" in q_upper or "COUNT(" in q_upper:
            # Check if SELECT follows directly inside the aggregate paren
            import re
            if re.search(r'(SUM|AVG|MIN|MAX|COUNT)\s*\(\s*\(\s*SELECT', q_upper):
                raise TranspilationError("AggregateSubquery", "Complex subqueries inside aggregates are explicitly banned.")
                
        # 3. Explicitly ban User-Facing FIRST() to safely allow DuckDB's internal "first" scalar mapping
        import re
        if re.search(r'\bFIRST\s*\(', q_upper):
            raise TranspilationError("FIRST", "Aggregate function 'FIRST' is not in the whitelist")

    def _enforce_join_rules(self, query: str) -> bool:
        """Phase 3B: Strictly confines explicit joins to a single INNER equality bridge."""
        q_upper = query.upper()
        import re
        joins = re.findall(r'\bJOIN\b', q_upper)
        if not joins:
            return False
            
        if len(joins) > 1:
            raise TranspilationError("MultipleJoins", "A query may contain at most one JOIN operation under Phase 3B constraints.")
            
        if re.search(r'\b(LEFT|RIGHT|FULL|OUTER|CROSS|NATURAL)\s+(OUTER\s+)?JOIN\b', q_upper):
            raise TranspilationError("BannedJoinType", "Outer, Cross, and Natural joins are strictly banned under Phase 3B.")
            
        if re.search(r'\bUSING\s*\(', q_upper):
            raise TranspilationError("UsingClause", "USING clauses are explicitly banned. Use explicit ON equality predicates.")
            
        # 4. No mixed combinations with subqueries or aggregations on the first cross
        if re.search(r'\(\s*SELECT\b', q_upper):
            raise TranspilationError("MixedComplexity", "Joins combined with derived tables or subqueries are banned until independently proven.")
            
        if re.search(r'\b(GROUP\s+BY|SUM|AVG|MIN|MAX|COUNT)\b', q_upper):
            raise TranspilationError("MixedComplexity", "Joins combined with aggregations are banned pending Phase 3C.")
            
        return True

    def _traverse_relational_node(self, node):
        """Recursive parse of DuckDB relational nodes (AST-equivalent) from EXPLAIN FORMAT JSON."""
        name = node.get("name", "")
        extra_info = node.get("extra_info", {})
        
        # Verify whitelist nodes
        allowed_nodes = {"PROJECTION", "SEQ_SCAN ", "SEQ_SCAN", "FILTER", "HASH_GROUP_BY", "PERFECT_HASH_GROUP_BY", "UNGROUPED_AGGREGATE", "ORDER_BY", "LIMIT", "TOP_N", "HASH_JOIN", "STREAMING_LIMIT", "CROSS_PRODUCT"}
        
        if name == "WINDOW":
            raise TranspilationError("WindowFunction", "Window functions are explicitly banned under the Tract 2 Control Spec")
            
        if name == "CROSS_PRODUCT":
            # ONLY allowed if it's a scalar subquery. DuckDB enforces this via a specific projection error string limit.
            # Convert entire node tree to str to recursively check for the scalar artifact
            node_str = str(node)
            if "More than one row returned by a subquery" not in node_str and "scalar_subquery" not in node_str:
                raise TranspilationError("CROSS_PRODUCT", "Explicit Cross Joins are banned. CROSS_PRODUCT IR is only authorized for exact scalar subqueries")

        if "JOIN" in name:
            join_type = extra_info.get("Join Type", "INNER")
            
            # Subqueries explicitly resolve to SEMI, MARK, or ANTI joins.
            if join_type in ("SEMI", "MARK", "ANTI"):
                pass
            # DuckDB's optimizer translates some ORDER BY ... LIMIT queries into a TOP_N followed by a 
            # HASH_JOIN SEMI on rowid = rowid. We must allow this internal AST artifact.
            elif join_type == "SEMI" and "rowid = rowid" in extra_info.get("Conditions", ""):
                pass
            elif join_type == "INNER":
                # Phase 3B explicit Inner Join Constraints
                conditions = str(extra_info.get("Conditions", ""))
                if ">" in conditions or "<" in conditions or "!=" in conditions:
                    raise TranspilationError("NonEqualityJoin", "Only exact equality predicates are authorized for INNER JOINs")
                # Ensure nested loops have explicit equality (safeguard against comma cross joins dodging parser string nets)
                if name == "NESTED_LOOP_JOIN" and "=" not in conditions:
                     raise TranspilationError("NonEqualityJoin", "Only exact equality predicates are authorized for INNER JOINs")
            else:
                raise TranspilationError(name, "Joins outside the bounded Phase 3B inner equality matrix are banned.")
            
        if name and name not in allowed_nodes and name != "RESULT_COLLECTOR":
            raise TranspilationError(name, f"Relational IR '{name}' is explicitly banned under the Tract 2 Control Spec")
            
        # Check window functions or recursive mappings in projections
        if "Projections" in extra_info:
            projections = str(extra_info["Projections"]).upper()
            if "OVER (" in projections or "OVER(" in projections or "WINDOW" in projections:
                raise TranspilationError("WindowFunction", "Window functions are explicitly banned under the Tract 2 Control Spec")
        
        # Check Aggregates
        if "Aggregates" in extra_info:
            aggs = str(extra_info["Aggregates"])
            whitelist = {"sum", "avg", "min", "max", "count", "count_star", "first"}
            
            # Match formats like: "first"(#1) or sum(#1) 
            import re
            for func_call in re.findall(r'"?([a-zA-Z_]+)"?\(', aggs):
                if func_call.lower() not in whitelist:
                    raise TranspilationError(func_call.upper(), f"Aggregate function '{func_call.upper()}' is not in the whitelist")
                    
        for child in node.get("children", []):
            self._traverse_relational_node(child)

    def transpile(self, query: str) -> str:
        self._enforce_read_only(query)
        self._enforce_subquery_rules(query)
        self._enforce_join_rules(query)
        
        # 1. Ask duckdb for the IR schema (verifying parse exactness)
        try:
            # If the syntax is completely broken, duckdb will raise a Catalog/Parser error here
            res = self.conn.execute(f"EXPLAIN (FORMAT JSON) {query}")
        except duckdb.ParserException as e:
            raise TranspilationError("SyntaxError", f"Failed local DuckDB parse: {str(e)}")
        except duckdb.CatalogException as e:
            raise TranspilationError("TableMapping", f"Schema violation: {str(e)}")
            
        json_plan = res.fetchone()[1]
        try:
            plan_tree = json.loads(json_plan)
        except:
             raise TranspilationError("IROutputError", "Failed to deserialize DuckDB IR plan")

        # 2. Traverse tree enforcing explicit Whitelist
        if isinstance(plan_tree, list) and len(plan_tree) > 0:
            self._traverse_relational_node(plan_tree[0])
            
        # 3. Fallback Translation Engine - we have proven the query is perfectly safe SQL.
        # Now we apply dialect swaps. This prototype uses basic regex dialect swaps
        # because the query surface has explicitly rejected CTEs, window functions, etc.
        bq_sql = query
        
        # Basic translations that differ between engines. 
        # (Duckdb uinteger -> Bigquery INT64 matching is implicit in external tables).
        
        # Dialect swaps:
        # 1. duckdb date_trunc('hour', col) -> TIMESTAMP_TRUNC(col, HOUR)
        bq_sql = re.sub(
            r"date_trunc\('([^']+)',\s*([a-zA-Z0-9_]+)\)",
            lambda m: f"TIMESTAMP_TRUNC({m.group(2)}, {m.group(1).upper()})",
            bq_sql, flags=re.IGNORECASE
        )
        
        # 2. DuckDB double quotes for aliases -> BigQuery standard aliases
        # This is a basic swap; BigQuery supports backticks, but often standard quotes are fine.
        
        return bq_sql.strip()
        
    def execute_bounded(self, client, bq_sql: str, timeout: int = 30, max_results: int = 100):
        """
        Executes the transpiled query against BigQuery and forces 
        arrow_iterable chunking to prevent memory exhaustion on result retrieval.
        """
        # Controlled BQ execution utilizing PyArrow chunking for bounded memory footprint
        job = client.query(bq_sql)
        # We process the first chunk to ensure bounding behavior is engaged and return the table
        # In a real pipeline, the researcher would iterate over results_iterable pages.
        results_iterable = job.result(timeout=timeout, max_results=max_results).to_arrow_iterable()
        
        # Combine the chunks into a single table for local processing (simulating small/bounded analytical sets)
        import pyarrow as pa
        batches = list(results_iterable)
        if not batches:
            # Need a schema for empty results if needed, but for prototype we return None or empty
            return None
        return pa.Table.from_batches(batches)
