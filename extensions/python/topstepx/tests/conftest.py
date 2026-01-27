import pytest
import os
import sys
import asyncio

# Ensure src module is in path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from models import LoginRequest
from auth import authenticate

@pytest.fixture(scope="session")
def token() -> str:
    """Get Topstep session token via live login using Env Vars credentials."""
    username = os.environ.get("QUANUX_TOPSTEP__USERNAME")
    password = os.environ.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = os.environ.get("QUANUX_TOPSTEP__API_KEY")

    if not (username and api_key):
        pytest.skip("Missing Topstep credentials. Ensure QUANUX_TOPSTEP__USERNAME and QUANUX_TOPSTEP__API_KEY are set.")

    creds = LoginRequest(username=username, password=password if password else "", api_key=api_key)
    
    print(f"DEBUG: Logging in as {username} for tests...")
    try:
        # We need an event loop for the session functionality
        # pytest-asyncio handles async tests, but fixture setup might need explicit running if not async fixture or careful
        return asyncio.run(authenticate(creds))
    except Exception as e:
        pytest.fail(f"Test login failed: {e}")

