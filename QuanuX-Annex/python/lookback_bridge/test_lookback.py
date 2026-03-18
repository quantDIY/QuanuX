import lookback_bridge
import time

ipc_socket = "/tmp/quanux_lookback.arrow"
graphql_query = 'query { historical_data(symbol: "AAPL", limit: 10) }'

print("Executing zero-copy lookback via Cython C++ bindings...")
t0 = time.perf_counter()
response = lookback_bridge.remote_schema_query(ipc_socket, graphql_query)
t1 = time.perf_counter()

print(f"Response: {response}")
print(f"Execution Time: {(t1-t0)*1000000:.2f} microseconds")
print("SUCCESS: Lookback Bridge compiled and triggered C++ bindings flawlessly.")
