import sys
import os

# Ensure extensions path is available
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../extensions/python/wrappers")))

try:
    import quanux_duckdb as duckdb
except ImportError:
    print("Warning: quanux_duckdb not found. Using Mock for now if not built.")
    class MockDuckDB:
        def __init__(self, path): pass
    class MockConn:
        def query(self, sql): pass
    class MockModule:
        DuckDB = MockDuckDB
        Connection = MockConn
    duckdb = MockModule()

class Database:
    _instance = None
    
    def __init__(self):
        # Determine storage path
        storage_path = os.getenv("QUANUX_STORAGE_PATH", "quanux.duckdb")
        self.db = duckdb.DuckDB(storage_path)
        self.con = duckdb.Connection(self.db)
        print(f"DuckDB initialized at {storage_path}")

    @classmethod
    def get(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

def get_db():
    return Database.get()
