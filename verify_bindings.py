
import sys
import os

# Add the build directory to python path to find the .so module
# Adjust path as necessary based on where cmake outputs the library
sys.path.append(os.path.abspath("server/indicators/build/pybind"))

try:
    import quanux_indicators as qi
    print("SUCCESS: Imported quanux_indicators module")
except ImportError as e:
    print(f"FAILURE: Could not import quanux_indicators: {e}")
    sys.exit(1)

# 1. Test SMA
print("\n--- Testing SMA ---")
data = [10.0, 20.0, 30.0, 40.0, 50.0]
sma_res = qi.compute_sma(data, 3)
print(f"SMA(3) of {data}: {sma_res}")

if len(sma_res) == 5 and abs(sma_res[2] - 20.0) < 0.001:
    print("SMA Test PASSED")
else:
    print("SMA Test FAILED")

# 2. Test Market Profile
print("\n--- Testing Market Profile ---")
mp = qi.DenseMarketProfile(100.0, 1.0)
mp.process(100.0)
mp.process(100.0)
mp.process(102.0)
count_100 = mp.query(100.0)
print(f"TPO Count at 100.0: {count_100}")

if count_100 == 2:
    print("MarketProfile Test PASSED")
else:
    print("MarketProfile Test FAILED")
