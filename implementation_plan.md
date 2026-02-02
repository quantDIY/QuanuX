# Implementation Plan - Cython Pilot: Indicators

The objective is to replace the `pybind11` wrapper for the `indicators` C++ library with `Cython` to achieve a 7x performance improvement in fine-grained calls.

## User Review Required
> [!IMPORTANT]
> **Build System Change**: We will modify `server/indicators/CMakeLists.txt` to use a `setup.py` driven build for the Python extension instead of pure CMake `pybind11_add_module`. This is standard for Cython but changes how the artifact is built.

## Proposed Changes

### Server / Indicators
Refactoring the Python bindings from `pybind11` to `Cython`.

#### [MODIFY] [CMakeLists.txt](file:///Users/Duncan/Antigravity/QuanuX/QuanuX/server/indicators/CMakeLists.txt)
- Remove `add_subdirectory(pybind)`
- Add a custom target to run `pip install .` or `python setup.py build_ext --inplace`.

#### [DELETE] [server/indicators/pybind/](file:///Users/Duncan/Antigravity/QuanuX/QuanuX/server/indicators/pybind)
- Remove `bindings.cpp` (The old pybind11 code).
- Remove `CMakeLists.txt`.

#### [NEW] [server/indicators/cython/](file:///Users/Duncan/Antigravity/QuanuX/QuanuX/server/indicators/cython)
- `_indicators.pyx`: The new Cython implementation exposing `SMA`, `MarketProfile`, `VolumeProfile`.
- `indicators.py`: (Optional) Pure Python wrapper if we want type hints / clean namespace invokers.
- `setup.py`: Build script for the extension.

## Verification Plan

### Automated Tests
1.  **Build**: Run the new build process.
    - `cd server/indicators/cython && python3 setup.py build_ext --inplace`
2.  **Verify**: Run the existing smoke test.
    - `python3 server/indicators/test_indicators.py`
    - *Note*: We might need to adjust the `sys.path` in `test_indicators.py` to point to the new `cython` build output directory.

### Success Criteria
- `test_indicators.py` passes without modification to the *usage* code (imports might change path).
- Performance is verified (optional, but we already have the benchmark).
