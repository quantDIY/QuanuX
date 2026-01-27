import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from contracts import search_contracts

@pytest.mark.asyncio
async def test_search_contracts(token: str):
    """Verify we can search for contracts."""
    contracts = await search_contracts(token, search_text="NQ")
    
    assert contracts is not None
    assert isinstance(contracts, list)
    assert len(contracts) > 0
    
    first = contracts[0]
    assert "id" in first
    assert "name" in first
    print(f"✅ Found contract: {first['name']} (ID: {first['id']})")
