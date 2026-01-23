import sys
import os
import time

# Add wrapper to path
sys.path.append('extensions/python/wrappers')
import quanux_duckdb as duckdb

def main():
    print("Verifying DuckDB C++ Extension...")
    
    try:
        # Create In-Memory DB
        db = duckdb.DuckDB(":memory:")
        con = duckdb.Connection(db)
        
        # Create Table
        con.query("CREATE TABLE test (id BIGINT, val DOUBLE, msg VARCHAR)")
        
        # Use Appender
        appender = duckdb.Appender(con, "test")
        
        print("Appending 1000 rows...")
        start = time.time()
        for i in range(1000):
            appender.begin_row()
            appender.append_int64(i)
            appender.append_double(i * 1.5)
            appender.append_string(f"msg_{i}")
            appender.end_row()
        
        appender.flush()
        appender.close()
        end = time.time()
        print(f"Append complete in {end - start:.4f}s")
        
        # Verify Data
        # We don't have a read API bound yet (con.query just runs SQL), 
        # so we rely on no errors for now, or if we bound a way to read...
        # Our Connection::query wrapper in C++ returns void, checking for errors.
        # Let's bind a simple fetch one day, but for now proving it runs without error is good.
        con.query("SELECT count(*) FROM test")
        print("Query executed successfully.")
        
    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
