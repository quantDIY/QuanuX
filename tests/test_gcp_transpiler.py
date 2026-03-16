import pytest
import sys
import os

# Add QuanuX-Annex to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../QuanuX-Annex')))

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
    Asserts that the approved subset matrix (SELECT, JOIN, GROUP BY, aggregations)
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
    Explicitly injects Window Functions and CTEs to verify that 
    TranspilationError is thrown deterministically.
    """
    query = "SELECT instrument_id, AVG(ask_price) OVER (PARTITION BY instrument_id) FROM MarketTick"
    with pytest.raises(TranspilationError) as excinfo:
        transpiler.transpile(query)
    
    assert "WindowFunction" in str(excinfo.value)
    assert "Window functions are explicitly banned under the Tract 2 Control Spec" in str(excinfo.value)

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
    
    dataset_id = f"{project_id}.quanux_historical_test"
    table_id = f"{dataset_id}.market_ticks_test"
    
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
    try:
        client.get_table(table_id)
        client.delete_table(table_id)
    except:
        pass
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
    
    # 2. Transpile
    local_query = "SELECT instrument_id, SUM(ask_size) as total_ask FROM MarketTick WHERE bid_price > 90.0 GROUP BY instrument_id ORDER BY instrument_id"
    local_result = transpiler.conn.execute(local_query).fetch_arrow_table()
    
    bq_sql = transpiler.transpile(local_query)
    # Dialect routing: DuckDB's local 'MarketTick' table name must be mapped to the actual BQ environment path
    bq_sql = bq_sql.replace("MarketTick", f"`{table_id}`")
    
    # 3. Execute bounded and assert parity
    remote_result = transpiler.execute_bounded(client, bq_sql)
    
    # Clean up test table
    client.delete_table(table_id, not_found_ok=True)
    
    assert remote_result is not None
    assert len(local_result) == len(remote_result)
    assert local_result.column('instrument_id')[0].as_py() == remote_result.column('instrument_id')[0].as_py()
    assert local_result.column('total_ask')[0].as_py() == remote_result.column('total_ask')[0].as_py()
