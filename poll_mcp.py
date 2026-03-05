import sys
import time
import os

sys.path.append("/opt/quanux/nexus")
# Bind the security credential required by the mcp_server logic
os.environ["HASURA_GRAPHQL_ADMIN_SECRET"] = "sup3rs3cr3tQUANUX_001"

try:
    from mcp_server import read_historical_telemetry
except ImportError as e:
    print(f"Import Error: {e}")
    sys.exit(1)

print("Polling FastMCP bridge for telemetry data...")

for i in range(12):
    try:
        # Limit to 10 rows to verify the pipeline
        res = read_historical_telemetry(10)
        
        # Verify it translated to markdown properly (bypassing the GraphQL and Cython catches)
        if "No telemetry data found" not in res and "[FATAL]" not in res:
            print(f"\nSUCCESS on attempt {i+1}:")
            print("========================================")
            print(res)
            print("========================================")
            sys.exit(0)
            
        print(f"Attempt {i+1}/12 - Waiting for data cascade... ({res})")
    except Exception as e:
        print(f"Attempt {i+1}/12 - Exception: {e}")
        
    time.sleep(5)

print("\nTIMEOUT: No telemetry data cascaded after 60 seconds.")
sys.exit(1)
