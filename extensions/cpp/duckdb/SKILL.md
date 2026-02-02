---
name: DuckDB C++ Extension
description: Custom C++ bindings for DuckDB focusing on fast data ingestion.
---

# DuckDB C++ Extension (`duckdb_ext`)

A high-performance Cython wrapper around `duckdb`, exposing the `Appender` API for bulk ingestion. Built using DuckDB amalgamation for maximum portability.

## Installation
Run `python3 setup.py build_ext --inplace` in `extensions/cpp/duckdb/cython`.

## Usage
```python
import duckdb_ext

db = duckdb_ext.PyDuckDB(":memory:") # or path
con = duckdb_ext.PyConnection(db)
con.query("CREATE TABLE foo (i INT)")

appender = duckdb_ext.PyAppender(con, "foo")
appender.begin_row()
appender.append_int32(42)
appender.end_row()
appender.flush()
```

## Architecture
- **Language**: Cython (C++17)
- **Source**: Amalgamation (`duckdb.cpp` + `duckdb.hpp`)
- **Performance**: Direct C++ calls, no Pybind overhead.

