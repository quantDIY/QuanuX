import asyncio
import nats
import sys

async def run_nest():
    print(">>> INITIATING TIER 4 NEST SIMULATION (NODE: SFO) <<<")
    nc = await nats.connect("nats://localhost:4222")
    js = nc.jetstream()
    
    in_dark = False
    
    print(">>> SFO Nest online. Initiating Control Plane polling loop. <<<")
    try:
        while True:
            try:
                # Attempt to get the bucket and the key inside the loop
                kv = await js.key_value("quanux_tier1")
                entry = await kv.get("quanux.tier1.leader")
                leader = entry.value.decode()
                
                if in_dark:
                    print(f"\n[{leader}] Heartbeat Restored! Convergence complete. Resuming new structural entries.")
                    in_dark = False
                else:
                    print(f"Control Plane Healthy. Leader: {leader}. Ritchie FSM accepting entries.", end="\r")
                
            except (nats.js.errors.BucketNotFoundError, nats.js.errors.KeyNotFoundError):
                if not in_dark:
                    print("\nWARNING: KV Lock or Bucket Not Found. Control Plane Unreachable!")
                    print(">>> ENTERING THE LONG-DARK <<<")
                    print("Ritchie FSM Engaged: Halting new entries. Blindly executing active exits.")
                    in_dark = True
                else:
                    print("... The Long-Dark Continues ... Awaiting Control Plane Genesis.", end="\r")
            
            await asyncio.sleep(1)
            
    except Exception as e:
         print(f"Nest simulation error: {e}")
    finally:
         await nc.close()

if __name__ == "__main__":
    asyncio.run(run_nest())
