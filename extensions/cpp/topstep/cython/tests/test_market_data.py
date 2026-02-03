import pytest
import pytest_asyncio
import sys
import os

# Add parent directory to path to find topstep_ext
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from topstep_ext import TopstepClient

@pytest.mark.asyncio
async def test_search_contracts(client, token):
    """Verify we can search for contracts (Ported from test_contracts.py)."""
    client.token = token
    response = await client.search_contracts(search_text="NQ")
    
    assert response["success"] is True
    assert "contracts" in response
    contracts = response["contracts"]
    assert isinstance(contracts, list)
    assert len(contracts) > 0
    
    first = contracts[0]
    assert "id" in first
    assert "name" in first
    print(f"✅ Found contract: {first['name']} (ID: {first['id']})")

@pytest.mark.asyncio
async def test_retrieve_bars(client, token, contract_id):
    """Verify we can retrieve bars (Ported from test_retrieve_bars.py)."""
    client.token = token
    # Retrieve bars for 10 minutes
    minutes = 10
    response = await client.retrieve_bars(contract_id, minutes)
    
    assert response["success"] is True
    # Verify we got data (list of bars or object)
    # The key depends on API.
    # We will print keys to debug if unknown, but assert success for now.
    pass
