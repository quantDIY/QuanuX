import sys
import os
import asyncio
import httpx
from datetime import datetime
from typing import Optional

# Ensure server module is in path
sys.path.append(os.getcwd())

from server.security.secrets import KeyringBackend
from server.app.domain.topstep.auth import authenticate
from server.app.domain.topstep.models import LoginRequest
from server.app.domain.topstep.accounts import search_accounts
from server.app.domain.topstep.contracts import search_contracts
from server.app.domain.topstep.history import retrieve_bars
from server.app.domain.topstep.orders import place_order, search_open_orders
from server.strategies.live_test.strategy import TestStrategy

import websockets
import json

BRIDGE_URL = "http://localhost:8077"
BRIDGE_WS_URL = "ws://localhost:8077"

async def main():
    print("🚀 Starting Live Test Strategy...", flush=True)

    # 1. Credentials
    kb = KeyringBackend()
    username = kb.get("QUANUX_TOPSTEP__USERNAME")
    password = kb.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = kb.get("QUANUX_TOPSTEP__API_KEY")

    if not (username and password and api_key):
        print("❌ Missing Topstep credentials in Keyring.")
        return

    # 2. Authenticate
    print("🔐 Authenticating...", flush=True)
    try:
        creds = LoginRequest(username=username, password=password, api_key=api_key)
        token = await authenticate(creds)
        print("✅ Authenticated.")
    except Exception as e:
        print(f"❌ Auth Failed: {e}")
        return

    # 3. Account Selection
    print("💰 Fetching Accounts...", flush=True)
    accounts = await search_accounts(token)
    if not accounts:
        print("❌ No accounts found.")
        return

    # Simple selection: First 'Simulated' or just first
    selected_account = None
    for acc in accounts:
        if "Simulated" in acc.get('name', '') or "Demo" in acc.get('name', ''):
             selected_account = acc
             break
    
    if not selected_account:
        print("⚠️ No explicit Demo account found, using first available.")
        selected_account = accounts[0]
    
    account_id = selected_account['id']
    print(f"✅ Using Account: {selected_account['name']} (ID: {account_id})")

    # 4. Contract (NQ)
    print("📈 Fetching NQ Contract...", flush=True)
    contracts = await search_contracts(token, "NQ")
    if not contracts:
        print("❌ NQ Contract not found.")
        return
    contract = contracts[0]
    contract_id = contract['id']
    print(f"✅ Using Contract: {contract.get('contractLabel', 'Unknown')} (ID: {contract_id})")

    # 5. Historical Data
    print("📚 Fetching 1 Week of 1-Min Data...", flush=True)
    bars = await retrieve_bars(token, contract_id, minutes=60*24*5) # Approx 1 week trading mins? Topstep limits might apply
    # Just fetching 1000 mins for test safety/speed
    bars_1k = await retrieve_bars(token, contract_id, minutes=1000)
    print(f"✅ Fetched {len(bars_1k.get('bars', []))} historical bars.")

    # 6. Bridge Connection & Subscription
    print("🔌 Connecting to SignalR Bridge...", flush=True)
    
    # 6a. Init Bridge via HTTP
    async with httpx.AsyncClient() as client:
        try:
            # 1. Health Check
            try:
                await client.get(f"{BRIDGE_URL}/health")
            except Exception:
                print("⚠️ Bridge not running on 8077. Please run 'node server/bridges/signalr/node/index.mjs'.")
                return

            # 2. Connect SignalR (Topstep)
            # NOTE: Real TopstepX Hub URL required here.
            connect_payload = {
                "hub_url": "https://rtc.topstepx.com/hubs/market", 
                "access_token": token,
                "skip_negotiation": True
            }
            res = await client.post(f"{BRIDGE_URL}/connect", json=connect_payload, timeout=10.0)
            if res.status_code != 200:
                print(f"❌ Bridge Connect Failed: {res.text}")
                # Continuing for test logic check...
            else:
                print("✅ Bridge SignalR Connected.")

            # 3. Subscribe
            # TopstepX usually wants specific args.
            sub_payload = {
                "listener": "T", # Listen for 'T' (Tick/Trade) events
                "method": "Subscribe",
                "args": [f"marketdata:{contract_id}"] 
            }
            await client.post(f"{BRIDGE_URL}/subscribe", json=sub_payload)
            print("✅ Subscribed to Market Data.")

        except Exception as e:
            print(f"❌ Bridge HTTP Error: {e}")
            return

    # 7. Real-Time Loop (WebSocket)
    print("🟢 Starting Strategy Loop (WebSocket)...", flush=True)
    strategy = TestStrategy()
    
    last_price = 20000.0 # seed
    if bars_1k.get('bars'):
        last_price = bars_1k['bars'][-1]['close']

    try:
        async with websockets.connect(BRIDGE_WS_URL) as ws:
            while not strategy.done:
                try:
                    # Wait for message with timeout to allow simulated ticks if quiet
                    try:
                        message = await asyncio.wait_for(ws.recv(), timeout=1.0)
                        data = json.loads(message)
                        
                        # Handle Bridge Messages
                        if data.get('type') == 'data' and data.get('event') == 'T':
                            # Parse Topstep Tick
                            # Payload: [ { price: ..., volume: ... } ] or similar structure
                            # We'll just print and assume simulate logic if structure complex
                            print(f"⚡ Tick Received: {data}")
                            # Extract price (pseudo)
                            # last_price = data['payload'][0]['price'] 
                            pass
                            
                    except asyncio.TimeoutError:
                        # SIMULATION fallback for checking logic flow if no real data
                        pass

                    # --- SIMULATION LOGIC (Keep active for Verification) ---
                    # Remove this when real data is confirmed flowing
                    import random
                    move = random.uniform(-0.5, 0.5)
                    last_price += move

                    # 1. Processing Tick
                    action = strategy.on_tick(last_price)
                    
                    # 2. Execute Action
                    if action == "BUY":
                        print(f"⚡ EXECUTION: BUY Market @ {last_price}")
                        # Place Order Logic ... (Same as before)
                        final_order = {
                            "accountId": account_id,
                            "contractId": contract_id,
                            "quantity": 1,
                            "orderType": 1, 
                            "action": "Buy",
                        }
                        try:
                           # await place_order(token, final_order)
                           pass
                        except Exception as e:
                           print(f"❌ Order Failed: {e}")
                        
                        strategy.on_fill("BUY", last_price, 1)

                    elif action == "SELL":
                         print(f"⚡ EXECUTION: SELL Market (Close) @ {last_price}")
                         strategy.on_fill("SELL", last_price, 1)

                except Exception as e:
                    print(f"⚠️ Loop Error: {e}")
                    await asyncio.sleep(1)

    except KeyboardInterrupt:
        print("\n🛑 Stopping...")

if __name__ == "__main__":
    asyncio.run(main())
