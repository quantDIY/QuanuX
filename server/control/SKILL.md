---
name: quanux-control-plane
description: "Python Control Plane (Client Library for C++ Runtime)"
---

# Python Control Plane (`quanux.runtime`)

## Overview
The Control Plane facilitates the "Remote Control" of the C++ organism from Python.
*   **Architecture**: It is a **Client**, not a Wrapper. It sends NATS messages to the `quanux_supervisor`.
*   **Philosophy**: "Python Agility, C++ Speed". The UI/Strategy Builder remains in flexible Python, while the core runs in native C++.

## 1. Usage
Agents should use this library to verify system state or command new strategies.

```python
from server.control import RuntimeClient

async with RuntimeClient() as client:
    # 1. Command (Act)
    # Spawns a C++ strategy in the Execution Node
    await client.spawn_strategy("momentum_v1", "ESH5")

    # 2. Telemetry (Listen)
    async for log in client.subscribe_logs():
        print(log)
```

## 2. Core Concepts
*   **Decoupling**: The Python process (e.g. `streamlit`) never loads `libquanux.so`. If the Python UI crashes, the C++ trading engine relies on its own Supervisor and keeps running.
*   **NATS as API**: All communication happens over `sys.cmd.*` text streams.
