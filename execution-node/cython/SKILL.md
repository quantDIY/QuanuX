---
name: cython_execution_node
description: Detailed instructions for building, deploying, and managing the Cython Execution Node (Edge Architecture).
---

# Cython Execution Node

The **Cython Execution Node** is the high-performance "Edge" runtime for QuanuX. It runs strategies compiled as shared objects (`.so`) directly against native adapters (`DirectAdapter`) or via a mesh (`NATSAdapter`).

## 🏗️ Architecture

- **Runtime Core (`core.so`)**: The `CPython` event loop manager. It loads the strategy, initializes the adapter, and handles the `asyncio` lifecycle.
- **Adapter Interface (`adapter.so`)**: The Abstract Base Class (ABC) that all strategies program against. Ensures your strategy is portable between "Direct" (Venue) and "Relay" (NATS) modes without code changes.
- **Strategy Loader**: Uses `importlib` to dynamically load a compiled `.so` module and instantiate the `Strategy` class.

## 🚀 Strategy Development

Strategies are standard Cython classes. They must effectively "duck-type" the `Strategy` interface (implement `on_start`, `on_tick`, etc.).

### Example Strategy (`my_strategy.pyx`)

```python
# distutils: language = c++
# cython: language_level = 3

from adapter cimport Adapter
import asyncio

class Strategy:
    cdef public Adapter adapter
    cdef public object config

    async def on_start(self, Adapter adapter):
        """Called when the Core has connected to the Adapter."""
        self.adapter = adapter
        print(f"Strategy Live on: {adapter.name}")
        
        # Subscribe to Symbols through the Adapter abstract interface
        await self.adapter.subscribe(["NQ", "ES", "CL"])

    async def on_tick(self, dict tick):
        """High-Frequency Data Handler."""
        # 'tick' is a dictionary. In future v2, this will be a C-struct pointer for zero-copy.
        
        # Example: Simple Moving Average Logic (Pseudo)
        # if tick['symbol'] == "NQ" and tick['price'] > 20000:
        #     await self.entry_signal(tick)
        pass

    async def entry_signal(self, tick):
        order = {
            "symbol": tick['symbol'],
            "side": "BUY",
            "qty": 1,
            "type": "MARKET"
        }
        res = await self.adapter.place_order(order)
        print(f"Order Placed: {res}")
```

## 🛠️ Build & Deployment

The Node and Strategies must be built **in-place** for the target architecture (e.g., Linux/ARM64 for Edge, macOS/x64 for Dev).

### 1. Build the Node
```bash
cd execution-node/cython
python3 setup.py build_ext --inplace
# Generates: core.so, adapter.so, direct_adapter.so, nats_adapter.so
```

### 2. Build a Strategy
Create a `setup_strategy.py` for your strategy file:
```python
from setuptools import setup, Extension
from Cython.Build import cythonize

setup(
    ext_modules=cythonize(
        [Extension("my_strat", ["my_strat.pyx"])],
        language_level=3
    )
)
```
Run: `python3 setup_strategy.py build_ext --inplace` -> Generates `my_strat.so`.

## ⚙️ Running the Node

Running requires a lightweight Python script to bootstrap the Core.

**Script (`run_node.py`):**
```python
import asyncio
import sys
from core import Core
from direct_adapter import DirectAdapter
from topstep_ext import TopstepClient  # If using Direct Adapter

async def main():
    core = Core()
    
    # Configure Adapter (e.g., Direct Topstep)
    # In a real scenario, this is config-driven (YAML/JSON)
    ts_client = TopstepClient()
    adapter = DirectAdapter(venue="topstep", client=ts_client)
    core.set_adapter(adapter)
    
    # Load Strategy
    core.load_strategy("dist/my_strat.so")
    
    # Execute
    await core.run()

if __name__ == "__main__":
    asyncio.run(main())
```

## 🔌 Adapter Configurations

- **DirectAdapter**: Zero-latency. Requires `topstep_ext` or `rithmic_ext` in the `PYTHONPATH`. Connects directly to the venue APIs.
- **NATSAdapter**: Distributed. Connects to a NATS Jetstream cluster. Publishes orders to `orders.new` and listens to `market.data.>`.

## 🔍 Verification

Use the included CLI test to verify the runtime environment without a strategy:
```bash
python3 tests/test_node_cli.py
```
Expected Output:
```
MockAdapter Connected
Strategy Started...
MockAdapter Placed Order...
```
