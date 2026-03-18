import ingest_worker
import time

payload = b'{"Venue":"NASDAQ", "Symbol":"AAPL", "Price":150.00, "Size":100}'

t0 = time.perf_counter()
for _ in range(10000):
    result = ingest_worker.process_jetstream_payload(payload)
t1 = time.perf_counter()

print("Output sample:", result)
print(f"Time for 10000 iterations: {(t1-t0)*1000:.2f} ms")

expected = "**Venue:**NASDAQ|**Symbol:**AAPL|**Price:**150.00|**Size:**100"
assert result == expected, f"Validation failed! Expected '{expected}', got '{result}'"
print("SUCCESS: Cython Ingestion Worker test passed securely and formatted standard Aleph Markdown at line-rate.")
