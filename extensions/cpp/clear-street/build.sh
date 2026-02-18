#!/bin/bash
set -e

# Clear Street C++ Adapter Build Script
# Invoked by `quanuxctl ext install clear-street`

echo "[ClearStreet] Building Adapter..."

# Ensure build directory exists
mkdir -p build

# Configure CMake
# - DCMAKE_BUILD_TYPE=Release for performance
# - DQUANUX_TURBO=ON if env var set (handled by CMakeLists logic if needed)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

echo "[ClearStreet] Build Complete."
