import pytest
import os
import sys
from typing import Generator

# Ensure server module is in path
sys.path.append(os.getcwd())

from server.app.config import ServerConfig

@pytest.fixture(scope="session")
def config() -> ServerConfig:
    """Load server configuration."""
    return ServerConfig()

@pytest.fixture(scope="session")
def token(config: ServerConfig) -> str:
    """Get Topstep session token via live login using Config/Keyring credentials."""
    # 1. Use static token if provided
    # if config.topstep.session_token:
    #     # Simple check if it looks valid
    #     return config.topstep.session_token

    # 2. Otherwise try to login
    username = config.topstep.username
    password = config.topstep.password
    api_key = config.topstep.api_key

    if not (username and password and api_key):
        pytest.skip("Missing Topstep credentials. Run 'python -m server.cli.main secrets setup'.")

    from server.app.domain.topstep.auth import authenticate
    from server.app.domain.topstep.models import LoginRequest
    import asyncio

    creds = LoginRequest(username=username, password=password, api_key=api_key)
    
    print(f"DEBUG: Logging in as {username} for tests...")
    try:
        return asyncio.run(authenticate(creds))
    except Exception as e:
        pytest.fail(f"Test login failed: {e}")
