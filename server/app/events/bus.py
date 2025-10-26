"""
Event Bus Abstraction (scaffold)
- v0.0.1 ships a no-op MemoryBus for local testing.
- NATS JetStream impl will slot in behind the same interface later.
"""

from __future__ import annotations
from typing import AsyncIterator

class MemoryBus:
    async def publish(self, subject: str, payload: dict) -> None:
        # no-op placeholder
        return

    async def subscribe(self, subject: str) -> AsyncIterator[dict]:
        # demo generator (never yields in memory stub)
        if False:
            yield {}
        return
