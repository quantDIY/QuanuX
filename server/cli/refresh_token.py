import sys
import os
import requests
import json

# Ensure server module is in path
sys.path.append(os.getcwd())

from server.app.config import ServerConfig
from server.security.secrets import KeyringBackend

def main():
    print("🔄 QuanuX Token Refresh")
    print("=======================")
    
    # Load config (this will fetch username/api_key from Keyring)
    try:
        config = ServerConfig()
    except Exception as e:
        print(f"❌ Failed to load config: {e}")
        return

    username = config.topstep.username
    api_key = config.topstep.api_key
    base_url = config.topstep.base_api_url

    if not username or not api_key:
        print("❌ Missing Topstep credentials in Keyring.")
        print("   Run 'python -m server.cli.setup_secrets' or 'python -m server.cli.import_env' first.")
        return

    print(f"Authenticating as: {username}")
    
    url = f"{base_url}/api/Auth/loginKey"
    data = {"userName": username, "apiKey": api_key}
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json"
    }

    try:
        response = requests.post(url, headers=headers, json=data)
        if response.ok:
            result = response.json()
            token = result.get("token")
            if token:
                print("✅ Authentication successful!")
                
                # Save to Keyring
                kb = KeyringBackend()
                key = "QUANUX_TOPSTEP__SESSION_TOKEN"
                kb.set(key, token)
                print(f"✅ Saved session token to Keyring: {key}")
            else:
                print("❌ Authentication succeeded but no token returned.")
        else:
            print(f"❌ Authentication failed: {response.status_code} - {response.text}")
    except Exception as e:
        print(f"❌ Error during authentication: {e}")

if __name__ == "__main__":
    main()
