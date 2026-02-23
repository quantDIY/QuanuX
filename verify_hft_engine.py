
import asyncio
import nats
import struct
import time
import random

# MarketTick Structure (64-byte aligned)
# uint64_t local_rec_ts;   // 8
# uint64_t exchange_ts;    // 8
# double price;            // 8
# uint32_t size;           // 4
# uint32_t flags;          // 4
# uint32_t instrument_id;  // 4
# uint64_t internal_arrival_ts; // 8
# uint64_t processing_start_ts; // 8
# uint8_t _pad[8];         // 8 (Explicit) + 4 (Implicit after instrument_id)
# Total: 64 bytes

# Q=8, d=8, I=4. 
# Layout: Q Q d I I I [4x implicit] Q Q [8x explicit]
STRUCT_FMT = "QQdIII4xQQ8x"

async def run():
    nc = await nats.connect("nats://localhost:4222")
    
    print("Connected to NATS. Sending binary ticks to MARKET.BIN...")

    instrument_id = 101 # Example ID for "AAPL"
    price = 150.0

    start_time = time.time()
    
    for i in range(100):
        price += random.uniform(-0.05, 0.05)
        now_ns = int(time.time_ns())
        
        # Pack the struct
        # We leave internal timestamps 0, engine will fill them
        payload = struct.pack(STRUCT_FMT,
                              now_ns,       # local_rec_ts
                              now_ns,       # exchange_ts
                              price,        # price
                              100,          # size
                              0,            # flags
                              instrument_id,# instrument_id
                              0,            # internal_arrival_ts (placeholder)
                              0             # processing_start_ts (placeholder)
                              )
        
        await nc.publish("MARKET.BIN", payload)
        
        if i % 10 == 0:
            print(f"Sent tick {i}: Price {price:.2f}")
            
        # Burst simulation
        # await asyncio.sleep(0.0001) 

    # Determine throughput
    elapsed = time.time() - start_time
    print(f"Sent 100 ticks in {elapsed:.4f}s")
    
    await nc.close()

if __name__ == '__main__':
    asyncio.run(run())
