import pytest
import asyncio
from datetime import datetime, timedelta, timezone

# We use the same cleanup/setup logic as lifecycle but split into tests.
# Note: These tests depend on state (orders need to exist to be cancelled).
# Ideally we keep them independent or use a shared order fixture.
# Legacy test_orders.py likely ran them sequentially or created fresh orders.
# We will make each test self-contained (create -> action -> verify).

@pytest.mark.asyncio
async def test_place_order(client, token, account_id, contract_id):
    client.token = token
    limit_price = 30000 
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1, "side": 1, "size": 1, "limitPrice": limit_price,
        "stopPrice": None, "trailPrice": None
    }
    placed = await client.place_order(account_id, order)
    assert placed["success"] is True
    assert placed["orderId"] > 0

@pytest.mark.asyncio
async def test_search_open_orders(client, token, account_id, contract_id):
    client.token = token
    # Place one first to ensure there is something
    await test_place_order(client, token, account_id, contract_id)
    
    res = await client.search_open_orders(account_id)
    assert res["success"] is True
    assert "orders" in res

@pytest.mark.asyncio
async def test_modify_order(client, token, account_id, contract_id):
    client.token = token
    # Place
    limit_price = 30000 
    order = {
        "accountId": account_id, "contractId": contract_id,
        "type": 1, "side": 1, "size": 1, "limitPrice": limit_price,
    }
    placed = await client.place_order(account_id, order)
    order_id = placed["orderId"]
    
    # Modify
    mod = await client.modify_order(account_id, order_id, size=2, limitPrice=limit_price+100)
    assert mod["success"] is True

@pytest.mark.asyncio
async def test_cancel_order(client, token, account_id, contract_id):
    client.token = token
    # Place
    limit_price = 30000 
    order = {
        "accountId": account_id, "contractId": contract_id,
        "type": 1, "side": 1, "size": 1, "limitPrice": limit_price,
    }
    placed = await client.place_order(account_id, order)
    order_id = placed["orderId"]
    
    # Cancel
    can = await client.cancel_order(account_id, order_id)
    assert can["success"] is True
