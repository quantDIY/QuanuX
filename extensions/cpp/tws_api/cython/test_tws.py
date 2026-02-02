import sys
import os

# Point to current dir
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import tws_api
    print("SUCCESS: Imported tws_api")
except ImportError as e:
    print(f"ERROR: Could not import tws_api: {e}")
    sys.exit(1)

# Instantiate
print("Creating TwsAdapterWrapper...")
# host, port, clientId
client = tws_api.TwsAdapterWrapper("127.0.0.1", 7497, 1) # Values don't matter for mock
print("Created.")

# Connect (Mock always returns True)
res = client.connect()
print(f"Connect result: {res}")
if res is True:
    print("VERIFICATION PASSED: Connect returned True")
else:
    print(f"VERIFICATION FAILED: Connect returned {res}")

# Send Order
# id, symbol, side, qty, price
print("Sending Order...")
client.send_order(1001, "AAPL", "BUY", 10.0, 150.50)
print("Order Sent (No crash means success)")
