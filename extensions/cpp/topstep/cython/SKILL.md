---
name: topstep_cython
description: Instructions for using the TopstepX Cython Extension for trading and market data.
---

# TopstepX Cython Extension

This extension provides a high-performance, async interface to the TopstepX platform, replacing the legacy Node.js bridge.

## Capabilities

- **Market Data**: Real-time quotes and historical bars.
- **Trading**: Order placement (Limit, Market, Stop), Modification, and Cancellation.
- **Account**: Balance, Positions, and Trade History.

## Usage (Python)

```python
from topstep_ext import TopstepClient
import os

async def main():
    client = TopstepClient()
    
    # Login (Credentials via Env or Args)
    token = await client.login(
        os.getenv("QUANUX_TOPSTEP__USERNAME"),
        os.getenv("QUANUX_TOPSTEP__PASSWORD"),
        os.getenv("QUANUX_TOPSTEP__API_KEY")
    )
    
    # 1. Search Accounts
    accounts = await client.search_accounts(only_active=True)
    account_id = accounts['accounts'][0]['id'] # Returns long long
    
    # 2. Market Data
    contracts = await client.search_contracts(search_text="NQ")
    contract_id = contracts['contracts'][0]['id']
    
    # 3. Order Placement
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1, # Limit
        "side": 1, # Buy
        "size": 1,
        "limitPrice": 15000.00
    }
    res = await client.place_order(account_id, order)
    print(f"Order Placed: {res}")
```

## Setup & Maintenance

- **Source**: `extensions/cpp/topstep/cython/`
- **Build**: `python3 setup.py build_ext --inplace`
- **Tests**: `pytest extensions/cpp/topstep/cython/tests/`

## Critical Notes
- **IDs**: All Account and Order IDs are `int64` (`long long`). Ensure your logic handles large integers.
- **Concurrency**: The client is fully async/await compatible.
