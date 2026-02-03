# Cython Execution Node: The "Edge" Architecture

## Overview

The **Cython Execution Node** is a specialized, high-performance runtime designed for "Edge" deployment. Unlike the general-purpose Go Execution Node (which excels at infrastructure, clustering, and reliable networking), the Cython Node is built for **pure speed** and **strategy logic execution**.

It allows you to run compiled strategies (`.so` files) natively effectively eliminating the latency overhead of interpreted Python while retaining the ease of development of the Python ecosystem.

## Core Philosophy

1.  **Zero-Latency Data Path**: 
    - When deployed with the `DirectAdapter`, market data from extensions (like Rithmic or Topstep) is passed directly to the strategy's memory space via C-pointers. There is no serialization (JSON/Protobuf) and no IPC (Inter-Process Communication) overhead.
2.  **Compilation as a Service**: 
    - Strategies are written in Cython (`.pyx`) and compiled to shared objects (`.so`). This protects intellectual property (binary form) and ensures optimal CPU instruction usage (AVX2, etc.).
3.  **Adapter Swapping**: 
    - The node uses an abstract `Adapter` pattern. The *same* strategy binary can run in two modes:
        - **Edge Mode**: Uses `DirectAdapter` to connect to the exchange API locally.
        - **Cloud Mode**: Uses `NATSAdapter` to subscribe to a data feed relayed from another node.

## Architecture

### 1. The Core (`core.so`)
The runtime engine. It initializes the `asyncio` event loop (compatible with our async Cython extensions), loads the configured Adapter, and manages the Strategy lifecycle.

### 2. The Adapter Interface (`adapter.so`)
An abstract base class that standardizes interaction. Strategies never import "Topstep" or "Rithmic" directly; they import `Adapter`.
- `connect()`: Establishes the link.
- `subscribe(symbols)`: Requests data.
- `place_order(order)`: execution.

### 3. Implementations
- **`DirectAdapter`**: Wraps our native extensions (`topstep_ext`, `rithmic_ext`).
- **`NATSAdapter`**: Speaks NATS protocol for distributed meshes.

## Developer Workflow

### 1. writing a Strategy
Create a simple `.pyx` file (e.g., `my_strategy.pyx`):

```python
# distutils: language = c++
from adapter cimport Adapter

class Strategy:
    async def on_start(self, Adapter adapter):
        print(f"Strategy started on {adapter.name}")
        await adapter.subscribe(["NQ", "ES"])

    async def on_tick(self, tick):
        # High-frequency logic here
        if tick['price'] > 20000:
            await self.adapter.place_order(...)
```

### 2. Compiling
Use the node's build system to compile your strategy into a standard artifact.

```bash
python3 setup_strategy.py build_ext --inplace
# Generates my_strategy.cpython-312-darwin.so
```

### 3. Deploying
Copy the `.so` file to the target edge server (e.g., an AWS t4g.micro instance in Chicago).

### 4. Running
Launch the node pointing to your strategy:

```bash
python3 run_node.py --strategy=my_strategy.so --adapter=direct --venue=topstep
```

## Performance Note
Because the entire stack (Extension -> Adapter -> Node -> Strategy) is compiled Cython, you avoid the **GI (Global Interpreter Lock)** for pure C++ operations and minimize Python object overhead. For heavy math, you can use `cdef` types and `nogil` blocks to achieve nearmetal performance.
