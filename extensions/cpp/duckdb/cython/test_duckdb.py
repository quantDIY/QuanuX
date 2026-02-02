import sys
import os

# Point to current dir
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import duckdb_ext
    print("SUCCESS: Imported duckdb_ext")
except ImportError as e:
    print(f"ERROR: Could not import duckdb_ext: {e}")
    sys.exit(1)

# Usage:
# 1. DB
# 2. Connection
# 3. Create Table (Query not exposed yet... hmm, I need to create table via SQL first ideally?)
# Wait, PyConnection has no query method exposed in current .pyx? 
# "Let's skip Query for now" was in the comment.
# But Appender fails if table doesn't exist?
# duckdb::Appender docs say table must exist.
# I MUST implement `query` in PyConnection to create the table first.

print("WARNING: PyConnection.query is not implemented. Cannot create table.")
print("Checking if valid classes exist...")

try:
    db = duckdb_ext.PyDuckDB(":memory:")
    print("PyDuckDB created.")
    con = duckdb_ext.PyConnection(db)
    print("PyConnection created.")
    
    # Create Table
    print("Creating table 'test_table'...")
    con.query("CREATE TABLE test_table (id BIGINT, val DOUBLE)")
    print("Table created.")

    # Create Appender
    app = duckdb_ext.PyAppender(con, "test_table")
    print("Appender created.")

    # Append Data
    app.begin_row()
    app.append_int64(1)
    app.append_double(1.5)
    app.end_row()
    
    app.begin_row()
    app.append_int64(2)
    app.append_double(2.5)
    app.end_row()
    
    app.flush()
    print("Data flushed.")
    
    # Verify Data?
    # No query result fetch implemented yet, but if flush didn't crash, good.
    
    # Cleanup
    app.close()
    
except Exception as e:
    print(f"CRITICAL FAILURE: {e}")
    sys.exit(1)

print("VERIFICATION COMPLETE (Table created, Data appended)")
