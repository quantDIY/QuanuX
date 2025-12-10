import sys
import os
from typing import Dict

# Ensure server module is in path
sys.path.append(os.getcwd())

from server.security.secrets import KeyringBackend

def parse_env_file(filepath: str) -> Dict[str, str]:
    """Parses a .env file into a dictionary."""
    secrets = {}
    if not os.path.exists(filepath):
        print(f"⚠️ File not found: {filepath}")
        return secrets

    with open(filepath, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            
            key, value = line.split("=", 1)
            key = key.strip()
            value = value.strip()
            
            # Remove quotes if present
            if (value.startswith('"') and value.endswith('"')) or \
               (value.startswith("'") and value.endswith("'")):
                value = value[1:-1]
                
            secrets[key] = value
    return secrets

def main():
    print("📥 QuanuX Secret Importer")
    print("=========================")
    
    env_path = ".env"
    print(f"Reading from: {env_path}")
    
    secrets = parse_env_file(env_path)
    if not secrets:
        print("No secrets found or empty file.")
        return

    kb = KeyringBackend()
    if not kb._keyring:
        print("❌ Keyring not available.")
        return

    # Define mapping or prefix rules
    # We want to map known keys to the QUANUX_ prefix if they aren't already
    # Or just import everything that looks like a secret?
    # The user said ".env contains keys needed for Topstep secrets".
    # Let's assume they might be named TOPSTEP_USERNAME etc. or QUANUX_TOPSTEP_USERNAME.
    
    count = 0
    for key, value in secrets.items():
        # If key doesn't start with QUANUX_, should we add it?
        # The config expects QUANUX_ prefix.
        
        target_key = key
        if not key.startswith("QUANUX_"):
            # Heuristic: if it looks like a config key, prefix it.
            # But maybe the user put "QUANUX_TOPSTEP_USERNAME" in .env directly?
            # Let's check.
            pass
            
        # Actually, let's just import them. But we need to match what ServerConfig expects.
        # ServerConfig expects QUANUX_TOPSTEP__USERNAME (double underscore for nested).
        # If the .env has TOPSTEP_USERNAME, we might want to map it to QUANUX_TOPSTEP__USERNAME?
        # Or maybe the user followed the old convention?
        # Let's try to be smart.
        
        if key == "TOPSTEP_USERNAME" or key == "USERNAME":
            target_key = "QUANUX_TOPSTEP__USERNAME"
        elif key == "TOPSTEP_API_KEY" or key == "API_KEY":
            target_key = "QUANUX_TOPSTEP__API_KEY"
        elif key == "SESSION_TOKEN":
            target_key = "QUANUX_TOPSTEP__SESSION_TOKEN"
        elif key == "MARKET_HUB_URL":
            target_key = "QUANUX_TOPSTEP__MARKET_HUB_URL"
        elif key == "TRADINGVIEW_CLIENT_ID":
            target_key = "QUANUX_TRADINGVIEW_CLIENT_ID"
        elif key == "BROKER_API_KEY":
            target_key = "QUANUX_BROKER_API_KEY"
        elif not key.startswith("QUANUX_"):
             # If it's just some random key, maybe prefix it? 
             # Safer to just import as is if we don't know, but Config won't pick it up unless prefixed.
             # Let's prefix it to be safe if it's upper case.
             if key.isupper():
                 target_key = f"QUANUX_{key}"

        try:
            kb.set(target_key, value)
            print(f"✅ Imported: {key} -> {target_key}")
            count += 1
        except Exception as e:
            print(f"❌ Failed to import {key}: {e}")

    print(f"\n✨ Imported {count} secrets to Keyring.")

if __name__ == "__main__":
    main()
