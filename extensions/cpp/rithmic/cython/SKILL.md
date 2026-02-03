---
name: rithmic_cython
description: Instructions for using the Rithmic Cython Extension (RApiPlus wrapper).
---

# Rithmic Cython Extension

This extension wraps the Rithmic RApiPlus C++ SDK, exposing `REngine` and callbacks to Python via Cython.

## Capabilities

- **Direct Memory Access**: Efficiently handles high-frequency market data.
- **RApiPlus Mapping**: Maps C++ structs (`LineInfo`, `TradeInfo`) to Python dictionaries.

## Usage (Python)

```python
from rithmic_ext import PyREngineParams, PyLoginParams, RCallbacksBase, PyREngine

# 1. Define Callbacks
class MyCallbacks(RCallbacksBase):
    def trade_print(self, info):
        print(f"Trade: {info['ticker']} @ {info['price']}")

# 2. Initialize Engine
params = PyREngineParams()
params.app_name = "QuanuX"
engine = PyREngine(params)

# 3. Login
login_params = PyLoginParams()
login_params.set_md_user("my_user")
# ... set other params ...

cb = MyCallbacks()
success, code = engine.login(login_params, cb)
```

## Setup & Maintenance

- **Source**: `extensions/cpp/rithmic/cython/`
- **Build**: `python3 setup.py build_ext --inplace`
- **Tests**: `pytest extensions/cpp/rithmic/cython/tests/`

## Requirements
- Requires valid Rithmic credentials and RApiPlus headers/libs available during build.
