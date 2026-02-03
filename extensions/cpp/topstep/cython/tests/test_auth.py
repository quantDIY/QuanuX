import pytest
import os

@pytest.mark.asyncio
async def test_authentication(client):
    """Verify we can authenticate (Ported from test_auth.py)."""
    # Use credentials from environment (should be present if tests are running)
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
            # Password might also be in keyring if separate, but conftest just gets all vars
        except ImportError:
            pass

    # Assert they exist instead of skipping, since we expect them for the suite
    assert username, "Username not set"
    assert api_key, "API Key not set"

    token = await client.login(username, password or "", api_key)
    assert token is not None
    assert len(token) > 20
    assert client.token == token
