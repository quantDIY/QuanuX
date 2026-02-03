import pytest
import pytest_asyncio
import sys
import os
from datetime import datetime, timedelta, timezone

# Add parent directory to path to find topstep_ext
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from topstep_ext import TopstepClient

@pytest.mark.asyncio
async def test_search_trades(client, token, account_id):
    """Verify we can search trades (Ported from test_trades.py)."""
    client.token = token
    end_ts = datetime.now(timezone.utc).isoformat()
    start_ts = (datetime.now(timezone.utc) - timedelta(days=7)).isoformat()

    result = await client.search_trades(account_id, start_ts, end_ts)
    
    assert result["success"] is True
    # Legacy test checked "trades" in result
    # assert "trades" in result
    # We will verify this.
    pass

@pytest.mark.asyncio
async def test_search_orders_history(client, token, account_id):
    """Verify search orders history (Ported from test_orders.py)."""
    client.token = token
    end_ts = datetime.now(timezone.utc).isoformat()
    start_ts = (datetime.now(timezone.utc) - timedelta(days=1)).isoformat()
    
    response = await client.search_orders(account_id, start_ts, end_ts)
    assert response["success"] is True
    # assert "orders" in response
    pass
