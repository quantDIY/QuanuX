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
    
    # Fallback to Keyring
    if not username:
        try:
            import keyring
            username = keyring.get_password("QuanuX", "QUANUX_TOPSTEP__USERNAME")
            password = keyring.get_password("QuanuX", "QUANUX_TOPSTEP__PASSWORD")
            api_key = keyring.get_password("QuanuX", "QUANUX_TOPSTEP__API_KEY")
        except ImportError:
            pass

    if not username or not api_key:
        pytest.skip("Missing Topstep credentials (Env: QUANUX_TOPSTEP__* or Keyring: QuanuX)")
        
    token = await client.login(username, password or "", api_key)
    return token

@pytest_asyncio.fixture
async def account_id(client, token):
    client.token = token
    accounts = await client.search_accounts(only_active=True)
    if not accounts["success"] or not accounts.get("accounts"):
        pytest.skip("No active accounts found.")
    print(f"DEBUG: Found accounts: {[a['id'] for a in accounts['accounts']]}")
    print(f"DEBUG: First account details: {accounts['accounts'][0]}")
    return accounts["accounts"][0]["id"]

@pytest_asyncio.fixture
async def contract_id(client, token):
    client.token = token
    # Try generic search for NQ
    contracts = await client.search_contracts(search_text="NQ")
    if not contracts["success"] or not contracts.get("contracts"):
         # Backup: Try "ES" if NQ not found
         contracts = await client.search_contracts(search_text="ES")
         if not contracts["success"] or not contracts.get("contracts"):
            pytest.skip("No contracts found for 'NQ' or 'ES'.")
    return contracts["contracts"][0]["id"]
