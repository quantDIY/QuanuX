# QUANUX-SUPERGRAPH(1)

## NAME
**quanux-supergraph** - CLI tools and diagnostics for the QuanuX superGraph telemetry pipeline.

## SYNOPSIS
A suite of commands to interact with the Aleph Protocol infrastructure, trigger the ETL memory bridge, and observe the Tier-1 data pipeline.

## DESCRIPTION
The QuanuX superGraph operates on a Tier-1 philosophy, rejecting "vendor magic" and implicit state. To interact with the physical metal (the V12 Matrix), operators utilize explicit scripts to move data across the network boundary, bypassing intermediate abstractions. This manual dictates the human-operator interface for managing the ingestion buffers and materializing the ephemeral cache.

## COMMANDS

### Triggering the DuckDB Materialization Bridge
The `materialize_bridge.py` script bridges cold S3 Parquet lakes into the bare-metal PostgreSQL 16 hot cache via DuckDB C-Bindings. This executes the "Midnight Pivot", syncing analytical data for the Hasura presentation layer.

**Execution:**
Ensure you are operating on the designated Oracle node (`panopticon-oracle`).
```bash
/opt/quanux/mcp_env/bin/python3 /path/to/materialize_bridge.py
```
**Expected Output:**
```
[*] Initiating DuckDB Materialized Bridge...
[*] Attaching to local Postgres daemon...
[*] Forging the Supergraph table...
[+] Materialization Complete. The Oracle is breathing.
```
*Note: Typical execution latency on the V12 matrix is ~0.34 seconds.*

### NATS JetStream Buffer Diagnostics
To verify that the ingestion buffer is actively listening and capable of surviving 10GB chaotic market swarms, perform a diagnostic port sweep of the Buffer node (`panopticon-buffer`).

**Execution (via Ansible):**
```bash
ansible -i dynamic_inventory.py panopticon-buffer -m shell -a "ss -tulpn | grep 4222" -b
```
**Expected Output:**
Ensure standard LISTEN socket bound to `0.0.0.0:4222` representing the NATS JetStream Docker container.

### FastMCP Cython Ping
To verify the final presentation layer is successfully compiling JSON to Markdown via the Cython shared objects:
```bash
/opt/quanux/mcp_env/bin/python3 verify_bindings.py
```

## ARCHITECTURE CONTEXT
The overall pipeline physically routes data through:
NATS (Ingestion) -> Vector (Router) -> GreptimeDB (Timeseries) -> MinIO S3 (Cold Storage) -> DuckDB (ETL Bridge) -> PostgreSQL (Hot Cache) -> Hasura (Supergraph) -> FastMCP (AI Translation).

## AUTHOR
The Architect & Dr. Antigravity

## SEE ALSO
`quanuxctl-obs(1)`, `quanux-translator(7)`
