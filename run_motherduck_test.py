import sys
import os

# Add paths
sys.path.append('extensions/python/wrappers')
sys.path.append('.')

import manage_keys
import quanux_duckdb as duckdb

def main():
    print("Verifying MotherDuck Integration...")
    
    # 1. Get Token
    try:
        # Try to get existing token or mock if not set (for CI/Test without real creds)
        token = manage_keys.keyring.get_password(manage_keys.SERVICE_NAME, "MOTHERDUCK_TOKEN")
        if not token:
            print("[WARN] MOTHERDUCK_TOKEN not found in keyring. Using mock token for test structure verification.")
            token = "ey...mock_token" # Won't actually connect, but verifies flow
            # In a real scenario, we'd exit or ask user to set it
    except Exception as e:
        print(f"Keyring error: {e}")
        return

    # 2. Connect to DuckDB
    print("Initializing DuckDB...")
    db = duckdb.DuckDB(":memory:")
    con = duckdb.Connection(db)

    # 3. Install/Load MotherDuck (Might fail if not built with extension support or no network)
    try:
        print("Installing/Loading motherduck extension...")
        # Note: In a static build environment ensuring these work is key
        # If this fails, we know we need to adjust CMake to allow dynamic extensions or link it statically
        con.query("INSTALL motherduck") 
        con.query("LOAD motherduck")
        print("MotherDuck extension loaded.")
    except Exception as e:
        print(f"[ERROR] Failed to load MotherDuck: {e}")
        print("Note: This is expected if 'libduckdb' was built without extension support or network is blocked.")
        # Proceeding to logic check only
    
    # 4. Set Token
    try:
        print("Setting Service Token...")
        con.query(f"SET motherduck_token='{token}'")
    except Exception as e:
        print(f"[ERROR] Failed to set token: {e}")

    # 5. Connect/Attach (Mock Check)
    # We won't actually attach to md: usually as it requires a valid token and network
    # But we can verify the SQL command behaves as expected (e.g. throws Auth error instead of Syntax error)
    if token != "ey...mock_token":
        try:
            print("Attempting to attach 'md:'...")
            con.query("ATTACH 'md:'")
            print("Successfully attached MotherDuck!")
        except Exception as e:
             print(f"Attach failed (likely auth/network): {e}")
    else:
        print("Skipping actual ATTACH 'md:' with mock token.")

    print("Verification Script Finished.")

if __name__ == "__main__":
    main()
