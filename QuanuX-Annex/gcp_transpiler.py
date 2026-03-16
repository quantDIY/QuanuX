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
            # All other non-select
            raise TranspilationError(q.split()[0] if q else "EMPTY", "Only SELECT statements are authorized")

    def _traverse_relational_node(self, node):
        """Recursive parse of DuckDB relational nodes (AST-equivalent) from EXPLAIN FORMAT JSON."""
        name = node.get("name", "")
        extra_info = node.get("extra_info", {})
        
        # Verify whitelist nodes
        allowed_nodes = {"PROJECTION", "SEQ_SCAN ", "SEQ_SCAN", "FILTER", "HASH_GROUP_BY", "PERFECT_HASH_GROUP_BY", "UNGROUPED_AGGREGATE", "ORDER_BY", "LIMIT"}
        
        if name == "WINDOW":
            raise TranspilationError("WindowFunction", "Window functions are explicitly banned under the Tract 2 Control Spec")
            
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
            whitelist = {"sum", "avg", "min", "max", "count"}
            
            # Match formats like: "first"(#1) or sum(#1) 
            for func_call in re.findall(r'"?([a-zA-Z_]+)"?\(', aggs):
                if func_call.lower() not in whitelist:
                    raise TranspilationError(func_call.upper(), f"Aggregate function '{func_call.upper()}' is not in the whitelist")
                    
        for child in node.get("children", []):
            self._traverse_relational_node(child)

    def transpile(self, query: str) -> str:
        self._enforce_read_only(query)
        
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
        
        # E.g time-series bucketing: date_trunc('hour', col) -> TIMESTAMP_TRUNC(col, HOUR)
        # Using a very simple regex for demonstration of prototype parsing
        bq_sql = re.sub(
            r"date_trunc\('([^']+)',\s*([a-zA-Z0-9_]+)\)",
            lambda m: f"TIMESTAMP_TRUNC({m.group(2)}, {m.group(1).upper()})",
            bq_sql, flags=re.IGNORECASE
        )
        
        # We also need to guarantee chunked, memory-safe data retrieval
        # The control spec says "Result Bounding: The class will output not just the SQL string, but a controlled BigQuery execution block"
        
        execution_block = f"""
# BQ Transpiled Query
query = \"\"\"
{bq_sql}
\"\"\"
# Controlled BQ execution utilizing PyArrow chunking for bounded memory footprint
job = client.query(query)
results_iterable = job.result().to_arrow_iterable()
# Bounded Arrow block pipeline...
"""
        return execution_block
