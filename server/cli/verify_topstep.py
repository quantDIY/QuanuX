import sys
import os
import asyncio
from typing import Optional

# Add server to path
sys.path.append(os.getcwd())

from server.security.secrets import KeyringBackend
from server.app.domain.topstep.auth import authenticate
from server.app.domain.topstep.models import LoginRequest

async def run_verification():
    print("🚀 Starting Topstep Verification...", flush=True)
    
    # 1. Retrieve Credentials
    kb = KeyringBackend()
    
    username = kb.get("QUANUX_TOPSTEP__USERNAME")
    password = kb.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = kb.get("QUANUX_TOPSTEP__API_KEY")
    
    missing = []
    if not username: missing.append("Username")
    if not password: missing.append("Password")
    if not api_key: missing.append("API Key")
    
    if missing:
        print(f"❌ Missing credentials: {', '.join(missing)}")
        print("Please run: python3 server/cli/main.py secrets setup")
        return

    print(f"✅ Credentials retrieved for user: {username}")

    # 2. Authenticate
    print("\n🔐 Authenticating with Topstep API...", flush=True)
    try:
        creds = LoginRequest(
            username=username,
            password=password,
            api_key=api_key
        )
        token = await authenticate(creds)
        
        print(f"✅ Authentication successful!")
        print(f"✅ Authentication successful!")
        print(f"   Token: ******** (masked)")
        
        # 3. Fetch Accounts
        from server.app.domain.topstep.accounts import search_accounts
        print("\n💰 Fetching Accounts...", flush=True)
        accounts = await search_accounts(token)
        if accounts:
            print(f"✅ Found {len(accounts)} accounts:")
            for acc in accounts[:3]: # Show first 3
                print(f"   - ID: {acc.get('id')} | Name: {acc.get('name')} | Type: {acc.get('accountType')}")
        else:
            print("⚠️ No accounts found (or API error).")

        # 4. Fetch Contracts (NQ)
        from server.app.domain.topstep.contracts import search_contracts
        print("\n📈 Fetching NQ Contracts...", flush=True)
        contracts = await search_contracts(token, search_text="NQ")
        if contracts:
            print(f"✅ Found {len(contracts)} contracts:")
            for c in contracts[:3]:
                print(f"   - {c.get('contractLabel')} (ID: {c.get('id')})")
        else:
            print("⚠️ No contracts found.")
        
    except Exception as e:
        print(f"❌ Authentication failed: {e}")

if __name__ == "__main__":
    asyncio.run(run_verification())
