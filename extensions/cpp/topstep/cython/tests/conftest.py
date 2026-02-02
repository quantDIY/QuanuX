import pytest
import pytest_asyncio
import os
import sys

# Add parent directory to path to find topstep_ext
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

from topstep_ext import TopstepClient

@pytest_asyncio.fixture
async def client():
    # Use default URL or from env
    base_url = os.getenv("QUANUX_TOPSTEP__BASE_API_URL", "https://api.topstepx.com")
    client = TopstepClient(base_url)
    return client

@pytest_asyncio.fixture
async def token(client):
    username = os.environ.get("QUANUX_TOPSTEP__USERNAME")
    password = os.environ.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = os.environ.get("QUANUX_TOPSTEP__API_KEY")
    
    if not username or not api_key:
        pytest.skip("Missing Topstep credentials (QUANUX_TOPSTEP__USERNAME/API_KEY)")
        
    token = await client.login(username, password or "", api_key)
    return token

@pytest_asyncio.fixture
async def account_id(client, token):
    # Ensure token is set on client (login does it, but to be sure for other tests)
    client.token = token
    accounts = await client.search_accounts(only_active=True)
    if not accounts["success"] or not accounts.get("items"):
        pytest.skip("No active accounts found.")
    return accounts["items"][0]["id"]

@pytest_asyncio.fixture
async def contract_id(client, token):
    client.token = token
    contracts = await client.search_contracts(search_text="NQ")
    if not contracts["success"] or not contracts.get("items"):
        pytest.skip("No contracts found for 'NQ'.")
    return contracts["items"][0]["id"]
