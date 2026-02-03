import pytest
import asyncio
from datetime import datetime, timedelta, timezone

@pytest.mark.asyncio
async def test_full_integration_flow(client, token, account_id, contract_id):
    """
    Verify full flow (Auth -> search -> place -> modify -> cancel).
    Ported from test_topstep_integration.py/test_order_lifecycle.
    """
    client.token = token
    
    # 1. Place Limit Order (Sell above market)
    limit_price = 30000 
    order = {
        "accountId": account_id,
        "contractId": contract_id,
        "type": 1, "side": 1, "size": 1, "limitPrice": limit_price,
        "stopPrice": None, "trailPrice": None
    }
    placed = await client.place_order(account_id, order)
    assert placed["success"] is True
    order_id = placed["orderId"]
    assert order_id > 0
    
    # 2. Check Open Orders
    await asyncio.sleep(1.0)
    open_orders = await client.search_open_orders(account_id)
    assert open_orders["success"] is True
    
    orders_list = open_orders.get("orders", [])
    found = any(str(o.get("id")) == str(order_id) or str(o.get("orderId")) == str(order_id) for o in orders_list)
    
    if not found:
        # Check history fallback from previous debugging
        from datetime import datetime, timedelta, timezone
        end_ts = datetime.now(timezone.utc).isoformat()
        start_ts = (datetime.now(timezone.utc) - timedelta(minutes=5)).isoformat()
        history = await client.search_orders(account_id, start_ts, end_ts)
        
        hist_orders = history.get("orders", [])
        hist_match = next((o for o in hist_orders if o["orderId"] == order_id), None)
        if hist_match and hist_match['status'] == 2:
             # Filled immediately
             return 

    assert found, "Order not found in open orders"

    # 3. Modify Order
    mod_resp = await client.modify_order(account_id, order_id, size=2, limitPrice=limit_price + 100)
    assert mod_resp["success"] is True
    
    # 4. Cancel Order
    await asyncio.sleep(0.5)
    cancel_resp = await client.cancel_order(account_id, order_id)
    assert cancel_resp["success"] is True or cancel_resp.get("errorCode") == 5
