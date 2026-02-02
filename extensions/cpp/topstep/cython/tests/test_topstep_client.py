import pytest
import asyncio
from datetime import datetime, timedelta, timezone

@pytest.mark.asyncio
async def test_authentication(client):
    """Verify we can authenticate."""
    # This test replicates logic from conftest but explicitly assertions
    import os
    username = os.environ.get("QUANUX_TOPSTEP__USERNAME")
    password = os.environ.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = os.environ.get("QUANUX_TOPSTEP__API_KEY")
    
    if not username or not api_key:
        pytest.skip("Missing credentials")

    token = await client.login(username, password or "", api_key)
    assert token is not None
    assert len(token) > 20
    assert client.token == token

@pytest.mark.asyncio
async def test_account_search(client, token):
    client.token = token
    response = await client.search_accounts(only_active=True)
    assert response["success"] is True
    assert "items" in response
    assert len(response["items"]) > 0

@pytest.mark.asyncio
async def test_contract_search(client, token):
    client.token = token
    response = await client.search_contracts(search_text="ES")
    assert response["success"] is True
    assert len(response["items"]) > 0

@pytest.mark.asyncio
async def test_order_lifecycle(client, token, account_id, contract_id):
    client.token = token
    
    # 1. Place Limit Order far away
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1,  # Limit
        "side": 1,  # Sell
        "size": 1,
        "limitPrice": 25000, 
        "stopPrice": None,
        "trailPrice": None
    }
    
    placed = await client.place_order(account_id, order)
    if not placed["success"]:
        pytest.fail(f"Failed to place order: {placed.get('error')}")
        
    order_id = placed["orderId"]
    assert order_id > 0
    print(f"Placed Order ID: {order_id}")
    
    # 2. Check Open Orders
    await asyncio.sleep(0.5)
    open_orders = await client.search_open_orders(account_id)
    assert open_orders["success"] is True
    # Verify our order is there
    found = any(o["orderId"] == order_id for o in open_orders.get("items", []))
    assert found, "Placed order not found in open orders"

    # 3. Modify Order
    mod_resp = await client.modify_order(account_id, order_id, size=2, limitPrice=25001)
    assert mod_resp["success"] is True
    
    # 4. Cancel Order
    await asyncio.sleep(0.5)
    cancel_resp = await client.cancel_order(account_id, order_id)
    assert cancel_resp["success"] is True or cancel_resp.get("errorCode") == 5

