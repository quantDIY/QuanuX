import pytest
import pytest_asyncio
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from datetime import datetime, timedelta, timezone
from trades import search_trades
from accounts import search_accounts

@pytest_asyncio.fixture
async def account_id(token: str):
    accounts = await search_accounts(token, only_active=True)
    if not accounts:
        pytest.skip("No active accounts found.")
    return accounts[0]["id"]

@pytest.mark.asyncio
async def test_search_trades(token: str, account_id: int):
    """
    Test the search_trades function to ensure it retrieves trade data successfully.
    """
    end_ts = datetime.now(timezone.utc).isoformat()
    start_ts = (datetime.now(timezone.utc) - timedelta(days=7)).isoformat()

    result = await search_trades(token, account_id, start_ts, end_ts)
    print("Search Trades Response:", result)

    assert result["success"] is True
    assert "trades" in result
