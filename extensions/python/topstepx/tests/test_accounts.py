import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from accounts import search_accounts

@pytest.mark.asyncio
async def test_search_accounts(token: str):
    """Verify we can fetch accounts."""
    accounts = await search_accounts(token, only_active=True)
    
    assert accounts is not None
    # We might not have active accounts, but it should return a list
    assert isinstance(accounts, list)
    
    if len(accounts) > 0:
        print(f"✅ Found {len(accounts)} accounts.")
        first = accounts[0]
        assert "id" in first
        assert "name" in first
    else:
        print("⚠️ No active accounts found (but API call succeeded).")
