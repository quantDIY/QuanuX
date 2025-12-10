import pytest
from server.app.domain.topstep.auth import authenticate
from server.app.domain.topstep.models import LoginRequest
from server.app.config import ServerConfig

@pytest.mark.asyncio
async def test_authentication(config: ServerConfig):
    """Verify we can authenticate with stored credentials."""
    username = config.topstep.username
    api_key = config.topstep.api_key
    
    if not username or not api_key:
        pytest.skip("Missing Topstep credentials in Keyring.")

    creds = LoginRequest(username=username, api_key=api_key)
    token = await authenticate(creds)
    
    assert token is not None
    assert len(token) > 0
    print(f"✅ Authenticated successfully. Token length: {len(token)}")
