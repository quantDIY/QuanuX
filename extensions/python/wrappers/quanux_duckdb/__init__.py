import os
import sys

# Add the directory containing the compiled .so to sys.path
build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../cpp/duckdb/build"))
if build_dir not in sys.path:
    sys.path.append(build_dir)

try:
    from duckdb_py import *
except ImportError as e:
    # If the extension hasn't been built yet, we might want to handle it gracefully
    # or just let it fail if the user expects it to be there.
    print(f"Warning: Could not import 'duckdb_py'. Make sure extensions/cpp/duckdb is built. Error: {e}")

# Re-export classes logic if needed, or simply star import does it.
# We can make the API more pythonic here if we wanted.
