import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))
from auth import authenticate
from models import LoginRequest


@pytest.mark.asyncio
async def test_authentication():
    """Verify we can authenticate with stored credentials."""
    username = os.environ.get("QUANUX_TOPSTEP__USERNAME")
    password = os.environ.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = os.environ.get("QUANUX_TOPSTEP__API_KEY")
    
    if not username or not api_key:
        pytest.skip("Missing Topstep credentials in Keyring/Env.")

    creds = LoginRequest(username=username, password=password if password else "", api_key=api_key)
    token = await authenticate(creds)
    
    assert token is not None
    assert len(token) > 0
    print(f"✅ Authenticated successfully. Token length: {len(token)}")
