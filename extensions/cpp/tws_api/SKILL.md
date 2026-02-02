---
name: tws_api_adapter
description: Polyglot (C++/Python) Adapter for IBKR TWS API. Requires SDK Injection.
---

# TWS API Adapter (Native + Python)

This extension wraps the **Native C++ TWS API**, providing access to Market Data, Algorithms, and Account updates that are not available via FIX.

## 💉 SDK Injection
This extension follows the **"Bring Your Own SDK"** model.
1.  Download the TWS API (Stable/Latest) from Interactive Brokers.
2.  Locate the `twsapi_macunix` folder (containing `IBJts`).
3.  Inject it into the QuanuX Ecosystem:
    ```bash
    quanuxctl integrate tws_api --path ~/Downloads/twsapi_macunix
    ```
    This copies the SDK to `extensions/sdks/twsapi`, making it available for builds.

## 🐍 Python Wrapper (Cython)
This extension uses **Cython** (`tws_api.pyx`) for a faster, more robust bridge to the C++ Adapter.

### Usage in Python
```python
import tws_api

# Connect to TWS (Port 7496) or Gateway (4001)
# Note: Cython wrapper uses mapped types
adapter = tws_api.TwsAdapter("127.0.0.1", 7496, 0)
adapter.connect()

# Send High-Performance Order
# ID, Symbol, Side, Qty, Price
# Side: 0=Buy, 1=Sell, 2=Short, 3=Cover
adapter.send_order(101, "AAPL", 0, 100, 150.00)
```

## C++ Usage
Link against the `tws_api_adapter` library. It implements the `QuanuX::IExecutionProvider` interface, allowing it to be swapped into any C++ Execution Node.
