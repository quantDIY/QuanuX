
import sys
import os
import asyncio
import json
import logging
from datetime import datetime

# Adjust path to include the current directory so local imports work if run as a script
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
sys.path.append(os.getcwd()) # For server package if run from root

# Use local topstep modules (packaged with strategy)
from topstep.auth import authenticate
from topstep.models import LoginRequest
from topstep.accounts import search_accounts
from topstep.contracts import search_contracts
from topstep.history import retrieve_bars
from server.security.secrets import KeyringBackend

# Import Strategy Logic
from strategy import MyStrategy

BRIDGE_URL = "http://localhost:8077"
BRIDGE_WS_URL = "ws://localhost:8077"

async def main():
    print("🚀 Starting TopstepGUIStrategy...", flush=True)

    # 1. Auth via Keyring
    kb = KeyringBackend()
    username = kb.get("QUANUX_TOPSTEP__USERNAME")
    password = kb.get("QUANUX_TOPSTEP__PASSWORD")
    api_key = kb.get("QUANUX_TOPSTEP__API_KEY")

    if not (username and password and api_key):
        print("❌ Missing Credentials in Keyring.")
        return

    print("🔐 Authenticating...", flush=True)
    try:
        creds = LoginRequest(username=username, password=password, api_key=api_key)
        token = await authenticate(creds)
        print("✅ Authenticated.")
    except Exception as e:
        print(f"❌ Auth Failed: {e}")
        return

    # 2. Account
    accounts = await search_accounts(token)
    if not accounts: return
    selected_account = accounts[0]
    print(f"✅ Using Account: {selected_account['name']} ({selected_account['id']})")

    # 3. Contract (NQ)
    contracts = await search_contracts(token, "NQ")
    if not contracts: return
    contract = contracts[0]
    print(f"✅ Using Contract: {contract.get('contractLabel', 'Unknown')} ({contract['id']})")

    # 4. Strategy Init
    strategy = MyStrategy()
    
    # 5. Connect to Bridge (SignalR)
    # Using httpx to init connection
    import httpx
    import websockets
    
    print("🔌 Connecting to SignalR Bridge...", flush=True)
    async with httpx.AsyncClient() as client:
        try:
             connect_payload = {
                "hub_url": "https://rtc.topstepx.com/hubs/market", 
                "access_token": token,
                "skip_negotiation": True
            }
             resp = await client.post(f"{BRIDGE_URL}/connect", json=connect_payload, timeout=10.0)
             if resp.status_code != 200:
                 print(f"❌ Connect Failed: {resp.text}")
             else:
                 print("✅ SignalR Connected.")
                 
             # Subscribe
             sub_payload = {
                "listener": "T",
                "method": "Subscribe",
                "args": [f"marketdata:{contract['id']}"]
             }
             await client.post(f"{BRIDGE_URL}/subscribe", json=sub_payload)
             print("✅ Subscribed.")
        except Exception as e:
            print(f"❌ Bridge Error: {e}")
            return

    # 6. WebSocket Loop
    from topstep.orders import place_order
    
    print("🟢 Strategy Loop Active...", flush=True)
    strategy = MyStrategy() # Re-init to be sure
    
    try:
        async with websockets.connect(BRIDGE_WS_URL) as ws:
            while True:
                try:
                    # Wait for message with short timeout to allow simulation if market closed
                    try:
                        msg = await asyncio.wait_for(ws.recv(), timeout=2.0)
                        data = json.loads(msg)
                        
                        if data.get('type') == 'data' and data.get('event') == 'T':
                            # Parse Topstep Tick
                            # Payload: [ { price: ..., volume: ... } ]
                            if 'payload' in data and len(data['payload']) > 0:
                                price = data['payload'][0].get('price')
                                if price:
                                    print(f"Tick: {price}")
                                    action = strategy.on_tick(price)
                                    if action == "BUY":
                                        print(f"⚡ SIGNAL: BUY @ {price}")
                                        
                                        # Execute Order
                                        order_req = {
                                            "accountId": selected_account['id'],
                                            "contractId": contract['id'],
                                            "quantity": 1,
                                            "action": "Buy",
                                            "orderType": 1 # Market
                                        }
                                        try:
                                            print("📤 Sending Order...")
                                            await place_order(token, order_req)
                                            print("✅ Order Sent (Check dashboard for Fill/Rejection)")
                                        except Exception as oe:
                                            print(f"⚠️ Order Rejected (Expected if Market Closed): {oe}")
                                            
                    except asyncio.TimeoutError:
                        # SIMULATION FALLBACK (Market Closed)
                        # Generate fake tick to verify logic path
                        import random
                        sim_price = 20000.0 + random.uniform(-5, 5)
                        # print(f"~ Sim Tick: {sim_price}") # Quiet sim
                        action = strategy.on_tick(sim_price)
                        if action == "BUY":
                             print(f"⚡ SIM-SIGNAL: BUY @ {sim_price}")
                             # Try sending real order even on sim signal to test API connectivity
                             order_req = {
                                "accountId": selected_account['id'],
                                "contractId": contract['id'],
                                "quantity": 1,
                                "action": "Buy",
                                "orderType": 1
                             }
                             try:
                                print(f"📤 Sending Test Order (Sim Signal)...")
                                await place_order(token, order_req)
                                print("✅ Order Sent")
                             except Exception as oe:
                                print(f"⚠️ Order Rejected (Expected): {oe}")
                                
                             # Pause to avoid spamming
                             await asyncio.sleep(5)

                except Exception as loop_e:
                    print(f"Loop Error: {loop_e}")
                    await asyncio.sleep(1)

                    
    except KeyboardInterrupt:
        print("Stopping.")

if __name__ == "__main__":
    asyncio.run(main())
