import sys
import os

# Resolve build path relative to this script: ../../../cpp/rithmic/build
script_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.abspath(os.path.join(script_dir, "../../../cpp/rithmic/build"))
print(f"Adding to sys.path: {build_dir}")
sys.path.append(build_dir)

try:
    import rithmic_py
    print(f"Rithmic Wrapper Version: {rithmic_py.version()}")
except ImportError as e:
    print(f"Failed to import rithmic_py: {e}")
    sys.exit(1)
