---
name: Rithmic C++ Extension
description: High-performance C++ bindings for Rithmic R | API+.
---

# Rithmic C++ Extension (`rithmic_ext`)

This extension provides high-performance Cython bindings for the Rithmic C++ SDK (`R | API+`). It replaces the previous Pybind11 implementation.

## Files
- `extensions/cpp/rithmic/cython/`: Cython source (`rithmic.pyx`) and `setup.py`.
- `extensions/cpp/rithmic/sdk/`: Rithmic C++ SDK (Headers/Libs).

## Installation
### Prerequisites
- **Linux**: `openssl`, `build-essential`. Rithmic SSL certs (`/path/to/certs`).
- **Mac**: `cmake` (for SDK management, if using tools), `setup.py` builds directly.

### Build
Run `python3 setup.py build_ext --inplace` inside `extensions/cpp/rithmic/cython/`. 
This compiles `rithmic_ext` linking against the static libraries in `../sdk`.

## Usage (Python)
```python
import rithmic_ext

class MyCallbacks(rithmic_ext.RCallbacksBase):
    def alert(self, info):
        print(f"Alert: {info['message']}")
    
    def best_bid_quote(self, info):
        print(f"Bid: {info['price']} x {info['size']}")

# Create Parameters
params = rithmic_ext.PyREngineParams()
params.app_name = "QuanuX"
params.app_version = "1.0.0"
params.log_file_path = "rithmic.log"

# Initialize Engine
engine = rithmic_ext.PyREngine(params)

# Login
lparams = rithmic_ext.PyLoginParams()
lparams.set_md_user("user")
lparams.set_md_password("pass")
lparams.set_md_cnnct_pt("server")
lparams.set_ts_user("user")
lparams.set_ts_password("pass")
lparams.set_ts_cnnct_pt("server")

engine.login(lparams, MyCallbacks())
engine.subscribe("MIME", "ESZ4", 0)
```

## Key Classes
- `PyREngine`: Main interface.
- `RCallbacksBase`: Base class for callbacks. Override `_on_*` methods are handled internally, just override `alert`, `line_update`, etc.
- `PyREngineParams`, `PyLoginParams`: Configuration objects.

## Thread Safety & GIL
Cython automatically manages the GIL.
- **Blocking Calls**: `login`, `subscribe` etc. release GIL where appropriate (handled by `REngine` C++ internally or wrapper). *Note: Ensure pure C++ blocking calls release GIL if long running.*
- **Callbacks**: The `CallbackShim` acquires GIL before calling Python methods.
- **Asyncio Bridge**: The Python `RithmicBridge` uses `loop.run_in_executor` for blocking calls and `loop.call_soon_threadsafe` for callbacks to ensuring safe integration with `asyncio`.

## Troubleshooting
### "os error" / "REngine Init Failed"
This usually means the Rithmic "Connection Point" files or SSL certificates are missing from the working directory.
- **Fix**: Ensure `rithmic_ssl_cert_auth_params` is present in `./ssl/` relative to where you run the application. Use `scripts/setup_rithmic_env.py` to link them.

