# distutils: language = c++
# cython: language_level = 3

from adapter cimport Adapter
import asyncio
import json
# Using nats-py for async support. 
# In future, we can wrap CNATS C++ lib for performance, but nats-py is fast enough for v1 glue.
# We treat 'nats' as a runtime dependency.

cdef class NATSAdapter(Adapter):
    """
    Adapter that talks to NATS.
    - Consumes Market Data (if Listener)
    - Publishes Orders (if Relay)
    """
    cdef public object nc
    cdef public str nats_url

    def __init__(self, str nats_url="nats://localhost:4222"):
        super().__init__(name="nats")
        self.nats_url = nats_url
        self.nc = None

    async def connect(self):
        import nats
        self.nc = await nats.connect(self.nats_url)
        print(f"Connected to NATS at {self.nats_url}")

    async def subscribe(self, list symbols):
        # Subscribe to NATS subjects like "market.data.>"
        for sym in symbols:
            await self.nc.subscribe(f"market.data.{sym}", cb=self._on_msg)

    async def _on_msg(self, msg):
        data = json.loads(msg.data.decode())
        await self.on_tick(data)

    async def place_order(self, dict order):
        # Publish order request to "orders.new"
        await self.nc.publish("orders.new", json.dumps(order).encode())
        return {"status": "sent_to_nats"}
