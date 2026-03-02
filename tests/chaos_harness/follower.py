import asyncio
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '../../')))

from server.ha.sentinel import GlobalSentinelLoop

async def run_follower():
    print(">>> INITIATING FOLLOWER SIMULATION (NODE: LON) <<<")
    sentinel = GlobalSentinelLoop(node_id="LON", nats_url="nats://localhost:4222")
    
    await sentinel.start()
    
    print(">>> LON Follower online. Watching KV Lock for failover. <<<")
    
    try:
        while True:
            await asyncio.sleep(1)
            # Log the law of verified death when transition occurs
            if sentinel._is_leader:
                 print("\n[CHAOS HARNESS] Follower successfully executed STONITH and assumed Leadership.")
                 break
                 
        # Keep alive as new leader
        while True:
            await asyncio.sleep(1)
    except KeyboardInterrupt:
        await sentinel.disconnect()

if __name__ == "__main__":
    asyncio.run(run_follower())
