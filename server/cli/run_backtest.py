
import asyncio
import os
import sys
from datetime import datetime
from server.strategies.full.SmaCrossover import SmaCrossoverStrategy
from server.app.domain.topstep.history import retrieve_bars

async def run_backtest():
    print("🚀 QuanuX Backtester: SmaCrossover (Live Data)")
    
    # Check for Token
    token = os.getenv("TOPSTEP_TOKEN")
    if not token:
        print("❌ Error: TOPSTEP_TOKEN environment variable not set.")
        print("Please export TOPSTEP_TOKEN='your_bearer_token'")
        sys.exit(1)

    # Config
    # NQ Contract ID (Example: 22485 for NQZ4? Need a real ID or user input)
    # 2024-12-20 Sample from docs used ID implicitly or showed it?
    # Let's default to a likely ID or ask user. 
    # For now, let's use a dummy ID and assume the user must provide a valid one if this fails, 
    # or rely on what's in 'retrieve_bars' logic if it has defaults (it doesn't).
    # Using 12345 as placeholder, user might need to change implementation or provide valid ID.
    contract_id = int(os.getenv("CONTRACT_ID", "12345")) 
    
    print(f"📥 Fetching Market Data for Contract {contract_id}...")
    
    # Fetch Data
    # Docs says: "limit": minutes in payload. 
    # Python wrapper function takes 'minutes' arg.
    # We want enough data for the SMA(20) to warm up.
    response_data = await retrieve_bars(token, contract_id, minutes=200)
    
    if not response_data.get("success"):
        print(f"❌ API Error: {response_data.get('errorMessage')}")
        return

    raw_bars = response_data.get("bars", [])
    print(f"✅ Received {len(raw_bars)} bars.")
    
    if not raw_bars:
        print("⚠️ No bars returned.")
        return

    # Normalize Data
    # API: { "t": "...", "o": 1.0, "h": 2.0, "l": 0.5, "c": 1.5, "v": 100 }
    # Strategy expects: { "close": 1.5, ... }
    bars = []
    for raw in raw_bars:
        bars.append({
            "timestamp": raw.get("t"),
            "open": raw.get("o"),
            "high": raw.get("h"),
            "low": raw.get("l"),
            "close": raw.get("c"),
            "volume": raw.get("v")
        })
        
    # Sort by timestamp just in case (API usually returns desc or asc? Docs don't specify, but usually desc? timestamps usually ASC for graphs)
    # The example response shows descending (14:00, 13:00, 12:00).
    # Time series analysis requires ASCENDING (oldest first).
    # Let's sort to be safe.
    bars.sort(key=lambda x: x["timestamp"])

    # Execute Strategy
    strategy = SmaCrossoverStrategy()
    print(f"🧠 Running Strategy {strategy.name}...")
    
    for bar in bars:
        strategy.on_bar(bar)
        
    print("\n🏁 Backtest Complete.")

if __name__ == "__main__":
    asyncio.run(run_backtest())
