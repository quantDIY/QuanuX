import sys
import os
import asyncio
import logging

# Add server to path
sys.path.append(os.getcwd())

from server.security.secrets import KeyringBackend

# Try to import async_rithmic (must be installed via pip install async-rithmic)
try:
    import async_rithmic
except ImportError:
    print("❌ 'async-rithmic' not found. Please install it: pip install async_rithmic")
    sys.exit(1)

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("verify_rithmic")

async def run_verification():
    print("🚀 Starting Rithmic Verification...", flush=True)
    
    # 1. Retrieve Credentials (Priority: Env -> Keyring)
    kb = KeyringBackend()
    
    username = os.getenv("QUANUX_RITHMIC_USER") or kb.get("QUANUX_RITHMIC_USER")
    password = os.getenv("QUANUX_RITHMIC_PASS") or kb.get("QUANUX_RITHMIC_PASS")
    system = os.getenv("QUANUX_RITHMIC_SYSTEM") or kb.get("QUANUX_RITHMIC_SYSTEM") or "Rithmic Test"
    # server = os.getenv("QUANUX_RITHMIC_SERVER") or kb.get("QUANUX_RITHMIC_SERVER") or "Orangeburg" # Not strictly used with direct URL
    
    if not username or not password:
        print("❌ Missing credentials in Keyring or Environment.")
        print("Run 'python3 server/cli/main.py secrets setup' or use the UI.")
        return

    print(f"✅ Using Credentials for User: {username}")
    print(f"   System: {system} (URL-based connection)")

    # 2. Connect
    print("\n🔐 Attempting Connection to Rithmic Plants...", flush=True)
    
    # NOTE: async-rithmic usually expects known server regions (Chicago, etc).
    # If "Orangeburg" isn't a standard key, we might need to look at how async-rithmic handles custom URIs.
    # The user provided wss://rituz00100.rithmic.com:443
    
    try:
        # User provided test URL: wss://rituz00100.rithmic.com:443
        # If 'Orangeburg' is just a label, we use the URL directly.
        
        client = async_rithmic.RithmicClient(
            url="wss://rituz00100.rithmic.com:443",
            user=username,
            password=password,
            app_name="QuanuX",
            app_version="17.9.0.0", # User provided file version 17.9.0.0
            system_name=system,
            # Server might not be needed if URL is provided, but keeping if kwarg allows
        )
        
        await client.connect()
        print("✅ Connection Successful!")
        
        # 3. List Accounts (Smoke Test)
        print("\n💰 Fetching Accounts...", flush=True)
        if client.accounts:
             print(f"✅ Found {len(client.accounts)} accounts.")
             for acc in client.accounts:
                 # Inspect object to find correct attributes if .name fails
                 acc_id = getattr(acc, 'account_id', 'Unknown')
                 acc_name = getattr(acc, 'name', getattr(acc, 'account_name', 'Unknown'))
                 print(f"   - {acc_id} ({acc_name})")
        else:
            print("ℹ️ No accounts loaded yet (or empty).")
            
        print("\n✅ Verification Complete. Disconnecting...")
        
    except Exception as e:
        print(f"❌ Connection Failed: {e}")
        print("\nDEBUG INFO:")
        print("If this failed on Server/Region lookup, async-rithmic might not know 'Orangeburg'.")
        print("We may need to map 'Orangeburg' to 'wss://rituz00100.rithmic.com:443' in the config.")

    finally:
        # Clean disconnect
        if 'client' in locals():
            try:
                await client.disconnect()
            except Exception:
                pass

if __name__ == "__main__":
    asyncio.run(run_verification())
