import asyncio
import json
import random
import nats

async def main():
    nc = await nats.connect("nats://localhost:4222")
    
    price = 5000.0
    print("Publishing market data to MARKET.ES...")
    
    for i in range(100):
        price += random.uniform(-1, 1)
        data = {
            "symbol": "ES",
            "price": price,
            "size": 1
        }
        await nc.publish("MARKET.ES", json.dumps(data).encode())
        await asyncio.sleep(0.01)
        
    print("Done publishing.")
    await nc.close()

if __name__ == '__main__':
    asyncio.run(main())
