#!/bin/bash
set -e

# Change directory to the script's location
cd "$(dirname "$0")"

# Allow version override from environment, default to v1.15.1
QUICKFIX_VERSION=${QUANUX_EXT_VERSION:-"v1.15.1"}
REPO_URL="https://github.com/quickfix/quickfix.git"
INSTALL_PREFIX="$(pwd)/dist"

echo "=========================================="
echo "Building QuickFIX Extension ($QUICKFIX_VERSION)"
echo "=========================================="

# 1. Clone or Update
if [ ! -d "source" ]; then
    echo "Cloning QuickFIX from $REPO_URL..."
    git clone --depth 1 --branch $QUICKFIX_VERSION $REPO_URL source
else
    echo "QuickFIX source already exists."
fi

# 2. Prepare Build Directory
rm -rf build
mkdir -p build
cd build

# 3. Configure CMake
# - CMAKE_BUILD_TYPE=Release: Performance optimization (Rule 0)
# - HAVE_PYTHON3=ON: Enable Python bindings
# - BUILD_SHARED_LIBS=OFF: Static linking for portability
# - CMAKE_INSTALL_PREFIX: Local install for C++ SDK
# - CMAKE_POLICY_VERSION_MINIMUM=3.5: Fix for newer CMake versions dropping legacy support
echo "Configuring CMake..."
PYTHON_EXEC=$(which python3)

cmake ../source \
    -DCMAKE_BUILD_TYPE=Release \
    -DHAVE_PYTHON3=ON \
    -DPython3_EXECUTABLE="$PYTHON_EXEC" \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_SHARED_LINKER_FLAGS="-undefined dynamic_lookup"

# 4. Build
echo "Building..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

# 5. Install C++ SDK (Headers & Libs)
echo "Installing C++ SDK to $INSTALL_PREFIX..."
make install

# 6. Install Python Bindings
# QuickFIX CMake installs python package to site-packages of the target python.
# Since we want it in the current venv, we might need a manual step if CMake doesn't handle venv detection perfectly.
# Usually, cmake install with proper Python3_EXECUTABLE does the right thing.
echo "Verifying Python installation..."
# Check if the module is importable
if $PYTHON_EXEC -c "import quickfix; print('QuickFIX imported successfully')" 2>/dev/null; then
    echo "Python bindings verified."
else
    echo "WARNING: Python bindings might not be in the path. Checking build artifacts..."
    # Manual install fallback if needed (copying .so/.py to site-packages)
    # But let's trust CMake install first.
    # If using local install prefix, CMake creates lib/pythonX/site-packages
    # We may need to copy that to the venv or add to PYTHONPATH.
    
    # Heuristic: Copy from dist/lib/python*/site-packages to venv
    SITE_PACKAGES=$($PYTHON_EXEC -c "import site; print(site.getsitepackages()[0])")
    echo "Deploying to $SITE_PACKAGES..."
    
    # Try to find the built python module in dist
    FOUND_PKG=$(find "$INSTALL_PREFIX/lib" -name "quickfix.py" | head -n 1)
    if [ -n "$FOUND_PKG" ]; then
        PKG_DIR=$(dirname "$FOUND_PKG")
        cp -v "$PKG_DIR"/* "$SITE_PACKAGES/"
        cp -v "$PKG_DIR"/_quickfix*.so "$SITE_PACKAGES/" 2>/dev/null || true
        cp -v "$PKG_DIR"/_quickfix*.dylib "$SITE_PACKAGES/" 2>/dev/null || true # MacOS
        cp -v "$PKG_DIR"/_quickfix.dylib "$SITE_PACKAGES/_quickfix.so" 2>/dev/null || true # MacOS Rename
        
        # Dependency Fix: Copy libquickfix.dylib from dist/lib to site-packages
        cp -v "$INSTALL_PREFIX/lib"/libquickfix*.dylib "$SITE_PACKAGES/"
        
        # RPATH Fix (MacOS): Tell _quickfix.so to look for libquickfix in the same directory
        install_name_tool -change "@rpath/libquickfix.16.dylib" "@loader_path/libquickfix.16.dylib" "$SITE_PACKAGES/_quickfix.so" || true
        
        echo "Manually deployed Python bindings with RPATH fix."
    else
        echo "Could not find built python bindings in dist. You may need to set PYTHONPATH manually."
    fi
fi

echo "=========================================="
echo "QuickFIX Build Complete"
echo "C++ SDK: $INSTALL_PREFIX"
echo "=========================================="
