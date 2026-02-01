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

## 🐍 Python Wrapper
This extension builds a **Pybind11** module (`TwsApi.so`) that exposes the C++ performance to Python strategies.

### Usage in Python
```python
import TwsApi

# Connect to TWS (Port 7496) or Gateway (4001)
adapter = TwsApi.TwsAdapter("127.0.0.1", 7496, 0)
adapter.connect()

# Send High-Performance Order
# ID, Symbol, Side, Qty, Price
adapter.send_order(101, "AAPL", "BUY", 100, 150.00)
```

## C++ Usage
Link against the `tws_api_adapter` library. It implements the `QuanuX::IExecutionProvider` interface, allowing it to be swapped into any C++ Execution Node.
