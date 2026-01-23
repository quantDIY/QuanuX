---
name: DuckDB C++ Extension
description: Custom C++ bindings for DuckDB focusing on fast data ingestion.
---

# DuckDB C++ Extension (`quanux_duckdb`)

A lightweight wrapper around `duckdb`, exposing the `Appender` API for high-performance row insertion. Designed to interop with other C++ extensions via `PyCapsule`.

## Installation
Run `extensions/cpp/duckdb/build.sh`. Fetches `libduckdb` from source.

## Usage
```python
import quanux_duckdb as duckdb

db = duckdb.DuckDB(":memory:") # or path
con = duckdb.Connection(db)
con.query("CREATE TABLE foo (i INT)")

appender = duckdb.Appender(con, "foo")
appender.begin_row()
appender.append_int32(42)
appender.end_row()
appender.flush()
```

## Interop
- `appender.get_capsule()` returns a `duckdb_appender` PyCapsule containing the `duckdb::Appender*`. 
- Pass this to other C++ extensions to allow them to write directly to the DB.
