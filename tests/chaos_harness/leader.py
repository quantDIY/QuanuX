import asyncio
import sys
import os

# Ensure the parent path is available for importing the actual Sentinel
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../')))

from server.ha.sentinel import GlobalSentinelLoop

async def run_leader():
    print(">>> INITIATING LEADER SIMULATION (NODE: NYC) <<<")
    # Using local NATS, replace URL if your nats-server runs elsewhere
    sentinel = GlobalSentinelLoop(node_id="NYC", nats_url="nats://localhost:4222")
    
    await sentinel.start()
    
    print(">>> NYC Leader running. Holding KV Lock. Kill process (Ctrl+C) to simulate node death. <<<")
    
    try:
        # Keep process alive
        while True:
            await asyncio.sleep(1)
    except KeyboardInterrupt:
        print("\n>>> SIMULATING HARD FAIL: Power lost or network partitioned! <<<")
        # DO NOT call sentinel.disconnect() here to simulate hard failure without lock release
        sys.exit(1)

if __name__ == "__main__":
    asyncio.run(run_leader())
