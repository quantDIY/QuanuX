import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from history import retrieve_bars
from contracts import search_contracts

@pytest.mark.asyncio
async def test_retrieve_bars(token: str):
    """Verify we can retrieve bars for a contract."""
    # First get a valid contract ID
    contracts = await search_contracts(token, search_text="NQ")
    if not contracts:
        pytest.skip("No contracts found, cannot test bar retrieval.")
    
    contract_id = contracts[0]["id"]
    print(f"Testing bar retrieval for contract ID: {contract_id}")

    # Retrieve bars
    bars_data = await retrieve_bars(token, contract_id=contract_id, minutes=10)
    
    assert bars_data is not None
    # The API structure for bars might vary, but let's check for common fields if successful
    # Usually it returns a list of bars or an object containing them
    print(f"Bars data keys: {bars_data.keys()}")
    
    # Adjust assertions based on actual response structure if needed
    # For now, just ensure we got a JSON response
    assert isinstance(bars_data, dict)
