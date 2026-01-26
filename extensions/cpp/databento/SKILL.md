---
name: Databento C++ Extension
description: Python bindings for Databento's C++ Historical API.
---

# Databento C++ Extension

Provides high-performance access to Databento's historical market data API using their C++ SDK. Includes a specialized "Connector" to stream data directly into DuckDB.

## Files
- `extensions/cpp/databento/`: Source, CMake config.
- `extensions/python/wrappers/databento/`: Python package (`databento_py`).

## Features
- **Historical Data**: `timeseries_get_range` with Python callbacks.
- **DuckDB Connector**: `timeseries_to_duckdb` for zero-copy-ish ingestion into DuckDB.
- **Types**: `TradeMsg`, `MboMsg`, `Record`.

## Usage
```python
import databento_py

# 1. Standard Usage
client = databento_py.HistoricalBuilder().set_key_from_env().build()
client.timeseries_get_range(..., callback=my_python_func)

# 2. DuckDB Pipe
# Requires quanux_duckdb extension appender
import quanux_duckdb
con = quanux_duckdb.Connection(...)
appender = quanux_duckdb.Appender(con, "table_name")
client.timeseries_to_duckdb(..., appender_capsule=appender.get_capsule())
client.timeseries_to_duckdb(..., appender_capsule=appender.get_capsule())

# 3. Live Data (Blocking)
# Supports both Real-time and Intraday Replay
builder = databento_py.LiveBuilder()
builder.set_key_from_env()
builder.set_dataset("GLBX.MDP3")

client = builder.build_blocking()
client.subscribe(["ESH5"], databento_py.Schema.Mbo, databento_py.SType.RawSymbol)
client.start()

for record in client:
    if record.rtype() == databento_py.RType.Mbo:
        msg = record.get_mbo()
        print(f"Price: {msg.price} Size: {msg.size}")
```
