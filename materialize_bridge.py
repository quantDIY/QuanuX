import duckdb

print('[*] Initiating DuckDB Materialized Bridge...')
con = duckdb.connect()

# Load native extensions
con.execute('INSTALL postgres; LOAD postgres;')

# DuckDB attaches to the local Postgres daemon
print('[*] Attaching to local Postgres daemon...')
con.execute("ATTACH 'host=127.0.0.1 port=5432 user=postgres password=postgres dbname=quanux' AS pg (TYPE postgres);")

# Create the schema and push the telemetry Parquet data natively into Postgres
print('[*] Forging the Supergraph table...')
con.execute("""
    CREATE TABLE IF NOT EXISTS pg.quanux_telemetry_live (
        timestamp TIMESTAMP,
        cpu_usage DOUBLE,
        memory_usage DOUBLE,
        latency_ns BIGINT
    );
""")

# Seed the matrix for the FastMCP ping
con.execute("INSERT INTO pg.quanux_telemetry_live VALUES (now(), 45.2, 60.1, 150000);")

print('[+] Materialization Complete. The Oracle is breathing.')
