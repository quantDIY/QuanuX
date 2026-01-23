#!/bin/bash
set -e

# Change directory to the script's location
cd "$(dirname "$0")"

# Detect OS
OS=$(uname -s)
echo "Building DuckDB extension for $OS..."

mkdir -p build
cd build


# Pass the current python executable to CMake to ensure version match
PYTHON_EXEC=$(which python3)
cmake -DPython3_EXECUTABLE="$PYTHON_EXEC" ..
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)
