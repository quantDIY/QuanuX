import pytest
import os
from server.app.config import ServerConfig
from server.app.domain.topstep.models import LoginRequest
from server.app.domain.topstep.auth import authenticate

@pytest.mark.asyncio
async def test_topstep_authentication_integration():
    """
    Integration test to verify connectivity to Topstep API.
    Requires QUANUX__TOPSTEP__USERNAME and QUANUX__TOPSTEP__API_KEY to be set.
    Skips if secrets are missing.
    """
    cfg = ServerConfig()
    
    if not cfg.topstep.username or not cfg.topstep.api_key:
        pytest.skip("Topstep credentials not configured. Set QUANUX__TOPSTEP__USERNAME and QUANUX__TOPSTEP__API_KEY to run this test.")
        
    creds = LoginRequest(
        username=cfg.topstep.username,
        api_key=cfg.topstep.api_key
    )
    
    print(f"\nAttempting to authenticate user: {creds.username} against {cfg.topstep.base_api_url}")
    
    try:
        token = await authenticate(creds, base_url=cfg.topstep.base_api_url)
        assert token is not None
        assert len(token) > 0
        print(f"Authentication successful! Token length: {len(token)}")
    except Exception as e:
        pytest.fail(f"Authentication failed: {str(e)}")
