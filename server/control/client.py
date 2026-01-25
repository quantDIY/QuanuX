
import asyncio
import logging
import json
import nats
from nats.errors import ConnectionClosedError, TimeoutError, NoRespondersError

logger = logging.getLogger(__name__)

class RuntimeClient:
    """
    Python Control Plane for the C++ QuanuX Runtime.
    
    This client does NOT look for local binaries or link C++ code.
    It strictly uses NATS to communicate with the 'quanux_supervisor' daemon.
    """
    def __init__(self, nats_url="nats://localhost:4222"):
        self.url = nats_url
        self.nc = None
        self.js = None

    async def connect(self):
        """Connect to the Nervous System."""
        try:
            self.nc = await nats.connect(self.url, name="quanux-python-ctl")
            self.js = self.nc.jetstream()
            logger.info(f"Connected to Runtime at {self.url}")
        except Exception as e:
            logger.error(f"Failed to connect to Runtime: {e}")
            raise

    async def close(self):
        if self.nc:
            await self.nc.close()

    async def spawn_strategy(self, strategy_name: str, symbol: str, runtime: str = "cpp"):
        """
        Command the Supervisor to spawn a strategy.
        
        Topic: sys.cmd.spawn
        payload: { "name": "...", "symbol": "...", "runtime": "cpp|python" }
        """
        if not self.js:
            raise ConnectionError("Not connected")

        payload = json.dumps({
            "cmd": "spawn",
            "name": strategy_name,
            "symbol": symbol,
            "runtime": runtime
        }).encode()

        try:
            # We publish to the Command stream
            ack = await self.js.publish("sys.cmd.spawn", payload)
            logger.info(f"Spawn command sent for {strategy_name} (Seq: {ack.seq})")
            return ack
        except Exception as e:
            logger.error(f"Failed to send spawn command: {e}")
            raise

    async def subscribe_logs(self):
        """
        Yields real-time logs from the C++ Supervisor.
        """
        if not self.nc:
            raise ConnectionError("Not connected")

        # Ephemeral subscription to system logs
        sub = await self.nc.subscribe("sys.log.>")
        try:
            async for msg in sub.messages:
                yield msg.data.decode()
        except Exception:
            pass

    async def __aenter__(self):
        await self.connect()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        await self.close()
