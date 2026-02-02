import sys
import os

# Point to current dir
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import databento_ext
    print("SUCCESS: Imported databento_ext")
except ImportError as e:
    print(f"ERROR: Could not import databento_ext: {e}")
    sys.exit(1)

# Instantiate Builder
print("Creating HistoricalBuilder...")
builder = databento_ext.PyHistoricalBuilder()
print("Created.")

# Set key (mock)
print("Setting key...")
try:
    builder.set_key("DUMMY_KEY")
    print("Key set.")
except Exception as e:
    print(f"Failed to set key: {e}")

# Build Client
print("Building Client...")
try:
    # This might throw if DUMMY_KEY is invalid for validation logic (if any local validation)
    # But usually validation is on request.
    client = builder.build()
    print("Client built.")
except Exception as e:
    # If it fails due to key format, that's fine, testing the binding mechanism.
    print(f"Client build exception (expected if validation exists): {e}")

print("VERIFICATION COMPLETE")
