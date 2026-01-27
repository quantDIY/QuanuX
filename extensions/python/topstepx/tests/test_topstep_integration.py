import pytest
import os
import sys

# Add the src directory to sys.path to allow importing the extension modules
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from models import LoginRequest
from auth import authenticate

@pytest.mark.asyncio
async def test_topstep_authentication_integration():
    """
    Integration test to verify connectivity to Topstep API.
    Requires QUANUX__TOPSTEP__USERNAME, QUANUX__TOPSTEP__PASSWORD, and QUANUX__TOPSTEP__API_KEY to be set.
    Skips if secrets are missing.
    """
    username = os.environ.get("QUANUX_TOPSTEP__USERNAME")
    password = os.environ.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = os.environ.get("QUANUX_TOPSTEP__API_KEY")
    base_url = os.environ.get("QUANUX_TOPSTEP__BASE_API_URL", "https://api.topstepx.com")
    
    if not username or not api_key:
        pytest.skip("Topstep credentials not configured. Set QUANUX_TOPSTEP__USERNAME and QUANUX_TOPSTEP__API_KEY to run this test.")
        
    creds = LoginRequest(
        username=username,
        password=password if password else "",
        api_key=api_key
    )
    
    print(f"\nAttempting to authenticate user: {creds.username} against {base_url}")
    
    try:
        # Assuming authenticate is imported correctly from ../src
        token = await authenticate(creds, base_url=base_url)
        assert token is not None
        assert len(token) > 0
        print(f"Authentication successful! Token length: {len(token)}")
    except Exception as e:
        pytest.fail(f"Authentication failed: {str(e)}")
