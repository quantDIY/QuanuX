import sys
import os

# Point to current dir (where .so is located)
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

try:
    import quanux_indicators
    print("SUCCESS: Imported quanux_indicators (Cython)")
except ImportError as e:
    print(f"ERROR: Could not import quanux_indicators: {e}")
    sys.exit(1)

# 1. Test SMA Class (Stateful)
print("\nTesting SMA Class (Cython Stateful implementation)...")
sma = quanux_indicators.SMA(5)
print(f"SMA(5) Created. Initial Value (Should be 0.0): {sma.value()}")

data = [1.0, 2.0, 3.0, 4.0, 5.0]
# Expected sequence:
# 1 -> 1/1 = 1.0 (if we average valid?) Or 1/5? 
# My Cython impl: sum/count.
# 1 -> 1/1 = 1.0
# 2 -> 3/2 = 1.5
# 3 -> 6/3 = 2.0
# 4 -> 10/4 = 2.5
# 5 -> 15/5 = 3.0

vals = []
for x in data:
    val = sma.update(x)
    vals.append(val)
    print(f"Added {x} -> SMA: {val}")

if vals[-1] == 3.0:
    print("VERIFICATION PASSED: SMA Final Value correct.")
else:
    print(f"VERIFICATION FAILED: SMA Expected 3.0, got {vals[-1]}")

# 2. Test Market Profile (Dense)
print("\nTesting DenseMarketProfile...")
mp = quanux_indicators.DenseMarketProfile(0.0, 0.1, 100)
mp.process(10.5) # 10.5
mp.process(10.5) # 10.5
mp.process(10.6) # 10.6

q1 = mp.query(10.5)
print(f"Query 10.5 -> {q1} (Expected 2)")

if q1 == 2:
    print("VERIFICATION PASSED: MarketProfile Query correct.")
else:
    print(f"VERIFICATION FAILED: MarketProfile Query expected 2, got {q1}")

# 3. Test Batch SMA
print("\nTesting compute_sma_batch...")
batch_res = quanux_indicators.compute_sma_batch(data, 5)
# output of compute_sma with period 5 on [1,2,3,4,5]
# Depending on SMA impl logic (NaN padding vs partial?):
# C++ sma_view implementation returns NaNs until period is met?
# Let's inspect the output.
print(f"Batch Result: {batch_res}")

if len(batch_res) == 5:
    print("VERIFICATION PASSED: Batch Result size correct.")
else:
    print(f"VERIFICATION FAILED: Batch Result size {len(batch_res)}")
