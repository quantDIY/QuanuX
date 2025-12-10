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
    """Get Topstep session token from Keyring."""
    token = config.topstep.session_token
    if not token:
        pytest.skip("No Topstep session token found. Run 'python -m server.cli.refresh_token' first.")
    return token
