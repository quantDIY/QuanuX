import pytest
from server.app.events.bus import MemoryBus

@pytest.mark.asyncio
async def test_memory_bus_publish():
    bus = MemoryBus()
    # Should be no-op and not raise error
    await bus.publish("test.subject", {"data": 1})

@pytest.mark.asyncio
async def test_memory_bus_subscribe():
    bus = MemoryBus()
    # Should be empty generator
    async for msg in bus.subscribe("test.subject"):
        pytest.fail("MemoryBus subscribe should not yield any messages")
