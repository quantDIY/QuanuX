# QuanuX Python Control Plane

The **Control Plane** (`quanux.runtime`) is the bridge between the Python ecosystem and the C++ Core.

## The Architecture: Best of Both Worlds
QuanuX decoupling the **User Interface** (Python) from the **Trading Engine** (C++).

1.  **Python Agility**: You build UIs, Dashboards, and Backtests in Python (Streamlit, Jupyter, Pandas).
2.  **C++ Speed**: The actual trading (Supervisor + Strategies) runs in a native, zero-overhead process.
3.  **The Bridge**: This library (`server/control/client.py`) sends commands over NATS.

## Usage

```python
from server.control import RuntimeClient

# Connect to the C++ Supervisor
client = RuntimeClient()
await client.connect()

# Tell the C++ engine to start a strategy
await client.spawn_strategy(
    strategy_name="scalper_v2",
    symbol="NQZ4"
)
```

## Why?
This design prevents "GIL Contention". A heavy calculation in your Python dashboard will **never** stall the heartbeat of the C++ execution node.
