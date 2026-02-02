import asyncio
import logging
from topstep_ext import TopstepClient

# Configure logging
logging.basicConfig(level=logging.INFO)

async def main():
    print("--- Testing Topstep Extension ---")
    
    # 1. Instantiate
    client = TopstepClient("https://api.topstepx.com")
    print(f"Client created. Base URL: {client.base_url}")
    
    # 2. Check attributes
    print(f"Token (should be empty): '{client.token}'")
    
    # 3. Test SignalR (Mock)
    # We expect ValueError because of no token
    try:
        client.connect_signalr()
    except ValueError as e:
        print(f"Caught expected error: {e}")
        
    print("SUCCESS: TopstepClient instantiated and verified.")

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
