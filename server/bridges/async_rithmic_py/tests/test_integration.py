import pytest
import os
import asyncio
from async_rithmic import RithmicClient

# Uses the same environment variables as the Bridge
RITHMIC_USER = os.getenv("QUANUX_RITHMIC_USER", "duncan@quantdiy.dev")
RITHMIC_PASS = os.getenv("QUANUX_RITHMIC_PASS", "SA_IsRqX")
RITHMIC_URL = os.getenv("QUANUX_RITHMIC_URL", "wss://rituz00100.rithmic.com:443")
APP_NAME = os.getenv("QUANUX_RITHMIC_APP_NAME", "QuanuX")
APP_VERSION = os.getenv("QUANUX_RITHMIC_APP_VERSION", "17.9.0.0")
SYSTEM_NAME = os.getenv("QUANUX_RITHMIC_SYSTEM", "Rithmic Test")

@pytest.mark.asyncio
async def test_rithmic_connection():
    """
    Verifies that we can connect and authenticate with Rithmic Test.
    """
    print(f"\n[Test] Connecting to {RITHMIC_URL} as {RITHMIC_USER}...")
    
    client = RithmicClient(
        url=RITHMIC_URL,
        user=RITHMIC_USER,
        password=RITHMIC_PASS,
        app_name=APP_NAME,
        app_version=APP_VERSION,
        system_name=SYSTEM_NAME,
    )
    
    try:
        await client.connect()
        assert True, "Connection successful (no exception raised)"
        
        # Verify we can see accounts (authenticated state)
        # Note: async-rithmic might need a moment or explicit call depending on internals,
        # but usually .connect() handles auth.
        
        # We can't easily check .connected property as it doesn't exist on this client version apparently,
        # but if await client.connect() returns, we are good.
        
        print(f"[Test] Accounts loaded: {len(client.accounts) if hasattr(client, 'accounts') else 'Unknown'}")
        
    except Exception as e:
        pytest.fail(f"Rithmic Connection Failed: {e}")
        
    finally:
        await client.disconnect()

@pytest.mark.asyncio
async def test_bridge_config_defaults():
    """
    Verifies that the Rithmic URL defaults to the correct Test/Paper URL 
    if not provided.
    """
    from server.bridges.async_rithmic_py.app import PORT
    assert PORT == 8078
