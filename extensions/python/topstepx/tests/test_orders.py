import pytest
import pytest_asyncio
import asyncio
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from datetime import datetime, timedelta, timezone
from orders import (
    search_orders,
    search_open_orders,
    place_order,
    cancel_order,
    modify_order
)
from accounts import search_accounts
from contracts import search_contracts

@pytest_asyncio.fixture
async def account_id(token: str):
    accounts = await search_accounts(token, only_active=True)
    if not accounts:
        pytest.skip("No active accounts found.")
    return accounts[0]["id"]

@pytest_asyncio.fixture
async def contract_id(token: str):
    contracts = await search_contracts(token, search_text="NQ")
    if not contracts:
        pytest.skip("No contracts found.")
    return contracts[0]["id"]

@pytest.mark.asyncio
async def test_search_orders(token: str, account_id: int):
    end_ts = datetime.now(timezone.utc).isoformat()
    start_ts = (datetime.now(timezone.utc) - timedelta(days=1)).isoformat()
    response = await search_orders(token, account_id, start_ts, end_ts)
    print(f"DEBUG: Search Orders Response: {response}")
    if not response["success"]:
        print(f"DEBUG: Error details: {response.get('error')}")
    assert response["success"] is True

@pytest.mark.asyncio
async def test_search_open_orders(token: str, account_id: int):
    response = await search_open_orders(token, account_id)
    print("Search Open Orders Response:", response)
    assert response["success"] is True

async def place_sample_order(token: str, account_id: int, contract_id: int):
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1,  # Limit
        "side": 1,  # Sell
        "size": 1,
        "limitPrice": 25000,  # High price to avoid immediate fill (NQ is ~20k)
        "stopPrice": None,
        "trailPrice": None,
        "customTag": None,
        "linkedOrderId": None
    }
    response = await place_order(token, order)
    print("Place Order Response:", response)
    assert response["success"] is True
    return response["orderId"]

@pytest.mark.asyncio
async def test_place_order(token: str, account_id: int, contract_id: int):
    await place_sample_order(token, account_id, contract_id)

@pytest.mark.asyncio
async def test_cancel_order(token: str, account_id: int, contract_id: int):
    order_id = await place_sample_order(token, account_id, contract_id)
    await asyncio.sleep(1.0)
    
    # Check order status
    open_orders = await search_open_orders(token, account_id)
    print(f"DEBUG: Open Orders: {open_orders}")
    
    response = await cancel_order(token, account_id, order_id)
    print(f"DEBUG: Cancel Order Response: {response}")
    # Accept success or error code 5 (Order not found / Too late)
    assert response["success"] is True or response.get("errorCode") == 5

@pytest.mark.asyncio
async def test_modify_order(token: str, account_id: int, contract_id: int):
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1,  # Limit
        "side": 0,  # Buy
        "size": 1,
        "limitPrice": 10000,  # Low price to avoid fill
        "stopPrice": None,
        "trailPrice": None,
        "customTag": None,
        "linkedOrderId": None
    }
    placed = await place_order(token, order)
    print("Place Order for Modify Response:", placed)
    assert placed["success"] is True
    order_id = placed["orderId"]
    
    await asyncio.sleep(0.5)
    response = await modify_order(token, account_id, order_id, size=2, limitPrice=5600)
    print("Modify Order Response:", response)
    assert response["success"] is True
    
    # Clean up
    await cancel_order(token, account_id, order_id)
