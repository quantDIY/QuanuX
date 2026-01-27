#!/bin/bash
set -e

# Ensure directories exist
mkdir -p execution-node/cpp/build_xcode
mkdir -p QuanuX-Backtesting-Engine/cpp/build_xcode

echo "Generating Xcode Project for Execution Node..."
cd execution-node/cpp/build_xcode
cmake .. -G Xcode
echo "Done. Open execution-node/cpp/build_xcode/QuanuX-Node.xcodeproj"

echo "Generating Xcode Project for Backtesting Engine..."
cd ../../../QuanuX-Backtesting-Engine/cpp/build_xcode
cmake .. -G Xcode
echo "Done. Open QuanuX-Backtesting-Engine/cpp/build_xcode/QuanuX-Backtester.xcodeproj"
