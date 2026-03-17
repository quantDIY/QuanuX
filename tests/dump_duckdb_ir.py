import duckdb
import json
import glob

def dump_ir(query, name):
    conn = duckdb.connect(':memory:')
    conn.execute("""
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
    res = conn.execute(f"EXPLAIN (FORMAT JSON) {query}")
    print(f"\n--- {name} ---")
    print(json.dumps(json.loads(res.fetchone()[1]), indent=2))

files = glob.glob('tests/fixtures/gcp_sql/*/*.sql')
for f in files:
    with open(f) as file:
        q = file.read().strip()
        dump_ir(q, f.split('/')[-1])
