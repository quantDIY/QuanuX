#!/bin/bash
set -e

# Colors
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${GREEN}Building QuanuX HFT Node...${NC}"

# Ensure we are in the script's directory
cd "$(dirname "$0")"

# Create build directory
mkdir -p build
cd build

# CMake Configure
echo "Configuring CMake..."
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    cmake -DCMAKE_BUILD_TYPE=Release ..
else
    # Mac/Other
    cmake -DCMAKE_BUILD_TYPE=Release ..
fi

# Build
echo "Compiling..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

echo -e "${GREEN}Build Complete.${NC}"
echo "Run with: ./build/quanux_node"
