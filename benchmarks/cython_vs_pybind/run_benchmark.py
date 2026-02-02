import timeit
import sys

# Ensure we can import the built modules
sys.path.append(".")

try:
    import bench_pybind
    print("Loaded bench_pybind")
except ImportError as e:
    print(f"Failed to load bench_pybind: {e}")

try:
    import bench_cython
    print("Loaded bench_cython")
except ImportError as e:
    print(f"Failed to load bench_cython: {e}")

N = 10_000_000

print(f"Running {N} iterations...")

# Baseline: Pure Python
py_stmt = "add(1, 2)"
py_setup = "def add(a, b): return a + b"
py_time = timeit.timeit(py_stmt, setup=py_setup, number=N)
print(f"Pure Python: {py_time:.4f}s ({N/py_time/1e6:.2f} M/ops)")

# Pybind11
if 'bench_pybind' in sys.modules:
    pb_stmt = "bench_pybind.add(1, 2)"
    pb_setup = "import bench_pybind"
    pb_time = timeit.timeit(pb_stmt, setup=pb_setup, number=N)
    print(f"Pybind11:    {pb_time:.4f}s ({N/pb_time/1e6:.2f} M/ops)")
else:
    pb_time = None

# Cython
if 'bench_cython' in sys.modules:
    cy_stmt = "bench_cython.add(1, 2)"
    cy_setup = "import bench_cython"
    cy_time = timeit.timeit(cy_stmt, setup=cy_setup, number=N)
    print(f"Cython:      {cy_time:.4f}s ({N/cy_time/1e6:.2f} M/ops)")
else:
    cy_time = None

if pb_time and cy_time:
    ratio = pb_time / cy_time
    print(f"\nSpeedup Cython vs Pybind11: {ratio:.2f}x faster")
