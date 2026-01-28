import logging
import asyncio
from nats.aio.client import Client as NATS
from nats.aio.errors import NatsError, ErrConnectionClosed, ErrTimeout, ErrNoServers
from ..config import ServerConfig

logger = logging.getLogger(__name__)

class NatsService:
    _instance = None
    
    def __new__(cls):
        if cls._instance is None:
            cls._instance = super(NatsService, cls).__new__(cls)
            cls._instance.nc = NATS()
            cls._instance._connected = False
            cls._instance.config = ServerConfig()
        return cls._instance

    async def connect(self):
        if self._connected:
            return

        try:
            # Only connect if configured to use NATS
            if self.config.event_bus != "nats":
                logger.info("NATS disabled via config (event_bus != nats)")
                return

            logger.info(f"Connecting to NATS at {self.config.nats_url}...")
            await self.nc.connect(
                servers=[self.config.nats_url],
                reconnect_time_wait=2,
                max_reconnect_attempts=60,
                error_cb=self._error_cb,
                closed_cb=self._closed_cb,
                reconnected_cb=self._reconnected_cb
            )
            self._connected = True
            logger.info("NATS connected successfully.")
        except (NoServersError, TimeoutError) as e:
            logger.error(f"Failed to connect to NATS: {e}")
            # We don't raise here to allow the server to start even if NATS is down
            # but functionality will be degraded.
        except Exception as e:
            logger.error(f"Unexpected NATS connection error: {e}")

    async def close(self):
        if not self._connected:
            return
        
        try:
            await self.nc.close()
            self._connected = False
            logger.info("NATS connection closed.")
        except Exception as e:
            logger.error(f"Error closing NATS connection: {e}")

    async def publish(self, subject: str, payload: bytes):
        if not self._connected:
            logger.warning(f"Cannot publish to {subject}: NATS not connected")
            return
        
        try:
            await self.nc.publish(subject, payload)
        except Exception as e:
            logger.error(f"Failed to publish to {subject}: {e}")

    async def subscribe_iterator(self, subject: str):
        """
        Yields messages from a subscription indefinitely.
        Useful for GraphQL Subscriptions.
        """
        if not self._connected:
            logger.warning(f"Cannot subscribe to {subject}: NATS not connected")
            return

        try:
            sub = await self.nc.subscribe(subject)
            logger.debug(f"Subscribed to {subject}")
            async for msg in sub.messages:
                yield msg
        except Exception as e:
            logger.error(f"Error in subscription {subject}: {e}")

    # Callbacks
    async def _error_cb(self, e):
        logger.error(f"NATS Connection Error: {e}")

    async def _closed_cb(self):
        logger.info("NATS Connection Closed")
        self._connected = False

    async def _reconnected_cb(self):
        logger.info("NATS Reconnected")
        self._connected = True
