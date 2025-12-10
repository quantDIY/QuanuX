import sys
import os
import getpass
from typing import List

# Ensure server module is in path
sys.path.append(os.getcwd())

from server.security.secrets import KeyringBackend

def prompt_secret(key_name: str, description: str) -> None:
    print(f"\n🔑 Setup {key_name} ({description})")
    print("   Leave empty to skip/keep existing.")
    
    value = getpass.getpass(f"   Enter value: ")
    if not value:
        print("   Skipped.")
        return

    kb = KeyringBackend()
    if not kb._keyring:
        print("❌ Keyring not available on this system.")
        return

    try:
        # We store keys with a prefix to match config expectations if needed,
        # but for simplicity, we'll store them as exact keys that the config loader will look for.
        # The config loader (KeyringSettingsSource) will likely look for "QUANUX__SECTION__KEY"
        # or we can just store them as "SECTION__KEY".
        # Let's use the env var naming convention for consistency: QUANUX__...
        
        full_key = f"QUANUX_{key_name}"
        kb.set(full_key, value)
        print(f"✅ Saved to Keyring: {full_key}")
    except Exception as e:
        print(f"❌ Failed to save: {e}")

def main():
    print("🔒 QuanuX Secure Setup")
    print("======================")
    print("This utility saves secrets to your OS Keyring.")
    print("They will be automatically loaded by the server.\n")

    # Define secrets to capture
    secrets = [
        ("TOPSTEP__USERNAME", "Topstep Username"),
        ("TOPSTEP__API_KEY", "Topstep API Key"),
        ("TRADINGVIEW_CLIENT_ID", "TradingView Client ID"),
        ("BROKER_API_KEY", "Broker API Key"),
    ]

    for key, desc in secrets:
        prompt_secret(key, desc)

    print("\n✨ Setup complete.")

if __name__ == "__main__":
    main()
