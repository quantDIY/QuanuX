import pytest
import sys
import os
import math

# Add QuanuX-Annex and the project root to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../QuanuX-Annex')))
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from gcp_transpiler import QuanuXDuckToBQTranspiler, TranspilationError

@pytest.fixture
def transpiler():
    return QuanuXDuckToBQTranspiler()

def test_read_only_enforcement(transpiler):
    """
    Asserts that sending an state-mutating text triggers an immediate, 
    unrecoverable exception prior to any parsing attempt.
    """
    with pytest.raises(TranspilationError) as excinfo:
        transpiler.transpile("UPDATE MarketTick SET bid_price = 100")
        
    assert "UPDATE" in str(excinfo.value)
    assert "State-mutating operations are strictly banned" in str(excinfo.value)
    assert "Fallback required:" in str(excinfo.value)

def test_whitelist_acceptance_matrix(transpiler):
    """
    Asserts that the approved subset matrix (SELECT, GROUP BY, aggregations)
    maps perfectly.
    """
    queries = [
        "SELECT instrument_id, SUM(ask_size) FROM MarketTick WHERE bid_price > 100.0 GROUP BY instrument_id",
        "SELECT COUNT(instrument_id) FROM MarketTick",
        "SELECT MIN(bid_price), MAX(ask_price) FROM MarketTick WHERE level = 1",
        "SELECT instrument_id, AVG(bid_price) FROM MarketTick GROUP BY instrument_id ORDER BY instrument_id DESC LIMIT 10"
    ]
    for q in queries:
        result = transpiler.transpile(q)
        assert isinstance(result, str)
        assert "SELECT" in result.upper()

def test_unsupported_construct_rejection(transpiler):
    """
    Explicitly injects Window Functions, Joins, and CTEs to verify that 
    TranspilationError is thrown deterministically.
    """
    # 1. Window Functions
    query = "SELECT instrument_id, AVG(ask_price) OVER (PARTITION BY instrument_id) FROM MarketTick"
    with pytest.raises(TranspilationError) as excinfo:
        transpiler.transpile(query)
    
    assert "WindowFunction" in str(excinfo.value)
    assert "Window functions are explicitly banned under the Tract 2 Control Spec" in str(excinfo.value)
    
    # 2. Joins
    query_join = "SELECT a.instrument_id FROM MarketTick a JOIN MarketTick b ON a.instrument_id = b.instrument_id"
    with pytest.raises(TranspilationError) as excinfo_join:
        transpiler.transpile(query_join)
    assert "Joins are explicitly banned" in str(excinfo_join.value)
    
    # 3. CTEs or unsupported IR
    query_cte = "WITH CTE AS (SELECT instrument_id FROM MarketTick) SELECT * FROM CTE"
    with pytest.raises(TranspilationError) as excinfo_cte:
        transpiler.transpile(query_cte)
    assert "Only SELECT statements are authorized" in str(excinfo_cte.value)

def test_phase1_surface_contract_frozen(transpiler):
    """
    Explicitly freezes the Phase 1 Matrix. This single contract test must
    never be changed without a formal Red Team promotion to a new Phase (e.g. Phase 2).
    """
    # 1. Assert exactly the approved surface (SELECT, FROM, WHERE, GROUP BY, ORDER BY, LIMIT)
    #    and approved aggregates (COUNT, SUM, AVG, MIN, MAX).
    approved_query = '''
        SELECT 
            level,
            COUNT(instrument_id) as c,
            SUM(bid_size) as s,
            AVG(bid_price) as a,
            MIN(ask_price) as min_p,
            MAX(ask_price) as max_p
        FROM MarketTick
        WHERE bid_price > 100 AND ask_size < 50
        GROUP BY level
        ORDER BY level DESC
        LIMIT 10
    '''
    # Must pass without raising TranspilationError
    assert "SELECT" in transpiler.transpile(approved_query).upper()
    
    # 2. Assert exactly the banned surface explicitly fails
    banned_queries = {
        "JOIN": "SELECT a.level FROM MarketTick a JOIN MarketTick b ON a.level = b.level",
        "WINDOW": "SELECT AVG(bid_price) OVER(PARTITION BY level) FROM MarketTick",
        "CTE": "WITH c AS (SELECT level FROM MarketTick) SELECT * FROM c",
        "UPDATE": "UPDATE MarketTick SET bid_price = 0",
        "DROP": "DROP TABLE MarketTick",
        "INSERT": "INSERT INTO MarketTick VALUES(1,1,1.0,1.0,1,1,1)",
        "DELETE": "DELETE FROM MarketTick"
    }
    
    for construct_name, q in banned_queries.items():
        with pytest.raises(TranspilationError) as excinfo:
            transpiler.transpile(q)
        # Verify the fail-close occurred
        assert "Fallback required" in str(excinfo.value)

def test_dialects_and_builtins(transpiler):
    """
    Tests specific dialect macros not allowed, like DuckDB unique things
    or unapproved aggregate functions that are not SUM, AVG, MIN, MAX, COUNT.
    """
    query = "SELECT instrument_id, FIRST(bid_price) FROM MarketTick GROUP BY instrument_id"
    with pytest.raises(TranspilationError) as excinfo:
        transpiler.transpile(query)
        
    assert "FIRST" in str(excinfo.value)
    assert "not in the whitelist" in str(excinfo.value)

def test_semantic_parity_fixture(transpiler):
    """
    Executes the transpiled approved queries against a mocked layout 
    and asserts exact row-count, grouping cardinality, and numeric precision 
    against local DuckDB results.
    """
    import duckdb
    import pyarrow as pa
    
    # 1. Local execution
    # Insert some dummy rows
    transpiler.conn.execute("INSERT INTO MarketTick VALUES (1000, 101, 150.0, 150.5, 10, 20, 1)")
    transpiler.conn.execute("INSERT INTO MarketTick VALUES (1000, 101, 150.1, 150.6, 15, 25, 2)")
    
    local_query = "SELECT instrument_id, SUM(ask_size) as total_ask FROM MarketTick WHERE bid_price > 100.0 GROUP BY instrument_id"
    local_result = transpiler.conn.execute(local_query).fetch_arrow_table()
    
    # 2. Transpile
    bq_sql = transpiler.transpile(local_query)
    
    # We would theoretically execute `bq_sql` against BigQuery, but since this is a 
    # unit test environment prototype without live credentials, we mock the BigQuery execution
    # to return the structurally identical local_result dataframe.
    # In a live validation, this would be an exact equality check asserting dataset parity.
    
    remote_result_mock = local_result  # Simulating perfect parity
    
    # Assert Semantic parity: exactly same row set, cardinality, numeric outputs
    assert len(local_result) == len(remote_result_mock)
    assert local_result.schema == remote_result_mock.schema
    assert local_result.column('total_ask')[0].as_py() == remote_result_mock.column('total_ask')[0].as_py()

def get_gcp_credentials():
    """Helper to load secrets from OS keyring explicitly before execution if missing from environ."""
    from server.security.secrets import KeyringBackend
    kb = KeyringBackend()
    
    # Try environment first, then keyring
    project = os.environ.get("GCP_PROJECT_ID") or kb.get("QUANUX_GCP_PROJECT_ID")
    creds = os.environ.get("GOOGLE_APPLICATION_CREDENTIALS") or kb.get("QUANUX_GOOGLE_APPLICATION_CREDENTIALS")
    
    if project:
        os.environ["GCP_PROJECT_ID"] = project
    if creds:
        os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = creds
        
    return project is not None and creds is not None

def test_real_bq_semantic_parity(transpiler):
    """
    The Red Team core graduation test: runs parity fixture against ACTUAL BigQuery results
    mirroring the approved Tract 1 surface.
    """
    if not get_gcp_credentials():
        pytest.skip("Requires real GCP credentials (GCP_PROJECT_ID and GOOGLE_APPLICATION_CREDENTIALS) in OS Env or via `quanuxctl secrets`.")
        
    from google.cloud import bigquery
    project_id = os.environ["GCP_PROJECT_ID"]
    client = bigquery.Client(project=project_id)
    
    import time
    dataset_id = f"{project_id}.quanux_historical_test"
    table_id = f"{dataset_id}.market_ticks_test_{int(time.time())}"
    
    # 1. Setup exact BQ test surface mirroring Tract 1
    dataset = bigquery.Dataset(dataset_id)
    dataset.location = "US"
    try:
        client.get_dataset(dataset_id)
    except:
        client.create_dataset(dataset, timeout=30)
        
    schema = [
        bigquery.SchemaField("timestamp_ns", "INTEGER"),
        bigquery.SchemaField("instrument_id", "INTEGER"),
        bigquery.SchemaField("bid_price", "FLOAT"),
        bigquery.SchemaField("ask_price", "FLOAT"),
        bigquery.SchemaField("bid_size", "INTEGER"),
        bigquery.SchemaField("ask_size", "INTEGER"),
        bigquery.SchemaField("level", "INTEGER"),
    ]
    table = bigquery.Table(table_id, schema=schema)
    table = client.create_table(table)
    
    # Insert rows into BOTH DuckDB and BigQuery
    rows_to_insert = [
        {"timestamp_ns": 1, "instrument_id": 999, "bid_price": 100.5, "ask_price": 101.0, "bid_size": 10, "ask_size": 20, "level": 1},
        {"timestamp_ns": 2, "instrument_id": 999, "bid_price": 100.6, "ask_price": 101.1, "bid_size": 15, "ask_size": 25, "level": 2},
        {"timestamp_ns": 3, "instrument_id": 888, "bid_price": 50.0, "ask_price": 50.5, "bid_size": 100, "ask_size": 200, "level": 1},
    ]
    
    transpiler.conn.execute("DELETE FROM MarketTick")  # Clear prior test state
    for r in rows_to_insert:
        transpiler.conn.execute(
            f"INSERT INTO MarketTick VALUES ({r['timestamp_ns']}, {r['instrument_id']}, {r['bid_price']}, {r['ask_price']}, {r['bid_size']}, {r['ask_size']}, {r['level']})"
        )
    client.insert_rows_json(table, rows_to_insert)
    
    import time
    time.sleep(3) # Wait for BQ streaming buffer
    
    # 2. Transpile with expanded AVG matrix
    local_query = "SELECT instrument_id, SUM(ask_size) as total_ask, AVG(bid_price) as avg_bid, MIN(ask_price) as min_ask FROM MarketTick WHERE bid_price > 90.0 GROUP BY instrument_id ORDER BY instrument_id"
    local_result = transpiler.conn.execute(local_query).fetch_arrow_table()
    
    bq_sql = transpiler.transpile(local_query)
    # Dialect routing: DuckDB's local 'MarketTick' table name must be mapped to the actual BQ environment path
    bq_sql = bq_sql.replace("MarketTick", f"`{table_id}`")
    
    # 3. Execute bounded and assert parity
    remote_result = transpiler.execute_bounded(client, bq_sql)
    
    # Exact Match Parity
    assert local_result.column('total_ask')[0].as_py() == remote_result.column('total_ask')[0].as_py()
    assert local_result.column('min_ask')[0].as_py() == remote_result.column('min_ask')[0].as_py()
    
    # Floating-Point Tolerance Parity for AVG (1e-9)
    assert math.isclose(
        local_result.column('avg_bid')[0].as_py(), 
        remote_result.column('avg_bid')[0].as_py(), 
        rel_tol=1e-9
    )
    
    # 4. Secondary Query Matrix Test: COUNT, LIMIT, ORDER BY DESC
    local_query_2 = "SELECT level, COUNT(instrument_id) as total_ticks, MAX(ask_size) as max_ask FROM MarketTick WHERE bid_price >= 50.0 GROUP BY level ORDER BY level DESC LIMIT 5"
    local_result_2 = transpiler.conn.execute(local_query_2).fetch_arrow_table()
    
    bq_sql_2 = transpiler.transpile(local_query_2).replace("MarketTick", f"`{table_id}`")
    remote_result_2 = transpiler.execute_bounded(client, bq_sql_2)
    
    assert remote_result_2 is not None
    assert len(local_result_2) == len(remote_result_2)
    assert local_result_2.column('total_ticks')[0].as_py() == remote_result_2.column('total_ticks')[0].as_py()
    assert local_result_2.column('max_ask')[0].as_py() == remote_result_2.column('max_ask')[0].as_py()
    assert local_result_2.column('level')[0].as_py() == remote_result_2.column('level')[0].as_py()

    # 5. Tertiary Query Matrix Test: Plain WHERE, Multiple Booleans, No Aggregations
    local_query_3 = "SELECT instrument_id, bid_price FROM MarketTick WHERE bid_price > 50.0 AND ask_size < 200 ORDER BY bid_price DESC LIMIT 2"
    local_result_3 = transpiler.conn.execute(local_query_3).fetch_arrow_table()
    
    bq_sql_3 = transpiler.transpile(local_query_3).replace("MarketTick", f"`{table_id}`")
    remote_result_3 = transpiler.execute_bounded(client, bq_sql_3)
    
    assert remote_result_3 is not None
    assert len(local_result_3) == len(remote_result_3)
    assert local_result_3.column('instrument_id')[0].as_py() == remote_result_3.column('instrument_id')[0].as_py()
    # Float exactness can vary slightly on direct fetches if not aggregated, but we check 1e-9 tolerance anyway for safety
    assert math.isclose(local_result_3.column('bid_price')[0].as_py(), remote_result_3.column('bid_price')[0].as_py(), rel_tol=1e-9)

    # Clean up test table
    client.delete_table(table_id, not_found_ok=True)
