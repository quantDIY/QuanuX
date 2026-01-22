#!/bin/bash
# Link the compile_commands.json from the build dir to the C++ source root
# This helps editors find it.

SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR/build"

if [ -f "compile_commands.json" ]; then
    echo "Linking compile_commands.json..."
    ln -sf "$PWD/compile_commands.json" ../
    echo "Done. IDE should now pick up include paths."
else
    echo "compile_commands.json not found. Run ./build.sh first."
fi
