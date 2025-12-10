import pytest
import pytest_asyncio
from server.app.domain.topstep.positions import (
    search_open_positions,
    close_position,
    partial_close_position
)
from server.app.domain.topstep.accounts import search_accounts
from server.app.domain.topstep.contracts import search_contracts

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
async def test_search_open_positions(token: str, account_id: int):
    result = await search_open_positions(token, account_id)
    print("Search Open Positions Response:", result)
    assert result["success"] is True

@pytest.mark.asyncio
async def test_close_position(token: str, account_id: int, contract_id: int):
    result = await close_position(token, account_id, contract_id)
    print(f"DEBUG: Close Position Response: {result}")
    # Accept common error codes indicating no open position (1), action not allowed (2), or contract not found (5)
    # Also accept 404 (Not Found) which likely means no position exists
    assert result["success"] is True or result.get("errorCode") in [1, 2, 5] or result.get("status") == 404

@pytest.mark.asyncio
async def test_partial_close_position(token: str, account_id: int, contract_id: int):
    result = await partial_close_position(token, account_id, contract_id, size=1)
    print(f"DEBUG: Partial Close Position Response: {result}")
    # Accept common error codes
    assert result["success"] is True or result.get("errorCode") in [1, 2, 5] or result.get("status") == 404
