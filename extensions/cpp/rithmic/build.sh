#!/bin/bash
set -e

# Directory checks
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
WRAPPER_DIR="${SCRIPT_DIR}/../../python/wrappers/rithmic"

echo "Building Rithmic Extension in ${BUILD_DIR}..."

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake
# Assuming Python3 is available in path or active venv
cmake ..

# Build
echo "Compiling..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# Copy artifact to Python wrapper
# The file extension might vary (.so on Linux, .dylib or .so on Mac depending on pybind11 config)
# Pybind11 usually produces .cpython-XY-darwin.so
echo "Copying artifacts to ${WRAPPER_DIR}..."
mkdir -p "$WRAPPER_DIR"

find . -name "rithmic_py*.so" -type f -exec cp {} "$WRAPPER_DIR/" \;

echo "Build complete. Artifacts copied."
