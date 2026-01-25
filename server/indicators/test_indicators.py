import sys
import os

# Point to build dir where .so is located (for development)
sys.path.append(os.path.join(os.path.dirname(__file__), "build"))

try:
    import quanux_indicators
    print("SUCCESS: Imported quanux_indicators")
except ImportError as e:
    print(f"ERROR: Could not import quanux_indicators: {e}")
    sys.exit(1)

# Test SMA
sma = quanux_indicators.SMA(5)
print(f"SMA(5) Created. Initial Value: {sma.value()}")

data = [1.0, 2.0, 3.0, 4.0, 5.0]
for x in data:
    val = sma.update(x)
    print(f"Added {x} -> SMA: {val}")

# Expected: (1+2+3+4+5)/5 = 3.0
if sma.value() == 3.0:
    print("VERIFICATION PASSED: SMA Value correct.")
else:
    print(f"VERIFICATION FAILED: Expected 3.0, got {sma.value()}")
