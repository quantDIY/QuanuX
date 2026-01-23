---
name: Rithmic C++ Extension
description: High-performance C++ bindings for Rithmic R | API+.
---

# Rithmic C++ Extension

This extension provides Python bindings for the Rithmic C++ SDK (`R | API+`). It allows for market data subscription, order management, and connection handling directly from Python with C++ performance.

## Files
- `extensions/cpp/rithmic/`: Source code.
- `extensions/python/wrappers/rithmic/`: Python wrapper package.
- `server/app/routers/rithmic.py`: FastAPI integration.

## Installation
### Prerequisites
- **Linux**: `openssl`, `cmake`, `build-essential`. Rithmic SSL certs (`/path/to/certs`).
- **Mac**: `cmake`.

### Build
Run `extensions/cpp/rithmic/build.sh`. This will unpack the SDK (checked into repo under `extensions/cpp/rithmic/sdk`) and compile `librithmic_py`.

## Usage (Python)
```python
import rithmic

class MyCallbacks(rithmic.RCallbacks):
    def alert(self, info):
        print(f"Alert: {info.message}")
    
    def best_bid_quote(self, info):
        print(f"Bid: {info.price} x {info.size}")

engine = rithmic.REngine(MyCallbacks())
engine.login(rithmic.LoginParams("user", "pass", "server", "app", "ver", "lic", "log_path"))
engine.subscribe("MIME", "ESZ4")
```

## Key Classes
- `REngine`: Main interface.
- `RCallbacks`: Override to handle async updates.
- `LoginParams`, `OrderParams`, `ModifyLimitOrderParams`: Structs for API calls.
