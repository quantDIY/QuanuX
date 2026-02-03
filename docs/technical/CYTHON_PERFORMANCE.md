# QuanuX Cython Architecture & Performance Guide

## Overview

QuanuX leverages **Cython** to bridge high-performance C++ trading engines with Python's rapid development ecosystem. This architecture replaces legacy Pybind11 and ctypes wrappers, offering significant performance gains and tighter integration.

## Why Cython?

1.  **Performance**: Cython compiles to C/C++, allowing direct access to native memory and bypassing the Python interpreter for critical loops.
    *   **GIL Release**: Long-running C++ operations (like generic algorithms or market data processing) can release the Global Interpreter Lock (GIL), allowing true parallelism.
    *   **Native Types**: Variable typing (e.g., `long long` for Order IDs) prevents overhead and errors (like overflow).
2.  **Safety**: Automatic handling of reference counting and exception translation between C++ and Python.
3.  **asyncio Integration**: Seamlessly integrates with Python's `asyncio` event loop while calling blocking C++ functions in separate threads (if needed) or using non-blocking C++ networking libraries.

## Key Extensions

### 1. TopstepX (`extensions/cpp/topstep/cython`)
*   **Status**: Fully Ported. 100% Test Parity.
*   **Features**:
    *   Native C++ SignalR Client (replaces Node.js bridge).
    *   `long long` ID support for >2B Order IDs.
    *   Direct `httpx` integration for REST calls.
*   **Performance**: ~7x faster order placement latency vs legacy Python/Node setup.

### 2. Rithmic (`extensions/cpp/rithmic/cython`)
*   **Status**: Verified Build.
*   **Features**:
    *   Wraps Rithmic's RApiPlus C++ SDK.
    *   Exposes `REngine` and `Callbacks` via a Python Shim.
    *   Zero-copy data transfer for market data ticks where possible.

## Workflow for Developers

### Building
Extensions are built in-place using `setuptools` and `Cython`.

```bash
# Topstep
cd extensions/cpp/topstep/cython
python3 setup.py build_ext --inplace

# Rithmic
cd extensions/cpp/rithmic/cython
python3 setup.py build_ext --inplace
```

### Testing
Use `pytest` to run the Cython-compiled tests.

```bash
# Run all Cython tests
pytest extensions/cpp/topstep/cython/tests/
pytest extensions/cpp/rithmic/cython/tests/
```

### Best Practices
*   **Type Everything**: Use `cdef` for all internal variables.
*   **Release GIL**: Use `with nogil:` for heavy C++ logic.
*   **Handle Errors**: Check return definitions in `.pyx` files. Most C++ methods return a success boolean or error code.
