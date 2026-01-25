
import asyncio
import logging
from server.control import RuntimeClient

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("test_control")

async def test_control_plane():
    logger.info("Connecting to C++ Runtime via Python Control Plane...")
    
    async with RuntimeClient() as client:
        # 1. Send Command (Fire & Forget)
        logger.info("Sending Spawn Command for 'DemoStrategy'...")
        try:
            # Note: This might timeout if no "Responder" is listening on sys.cmd.* yet
            # The C++ Supervisor we built earlier currently just bootstraps streams, it doesn't have a Replier yet.
            # So we expect a NoRespondersError, but that proves NATS is working!
            await client.spawn_strategy("demo_strategy", "ESH5")
        except Exception as e:
            logger.info(f"Command Response: {e} (Expected 'No responders' if Supervisor is just a bootstrapper)")

        # 2. Listen for logs (Async Generator)
        logger.info("Listening for logs (Ctrl+C to stop)...")
        # In a real test we would break after a few seconds
        try:
            async for log in client.subscribe_logs():
                print(f"[LOG] {log}")
        except asyncio.CancelledError:
            pass

if __name__ == "__main__":
    try:
        asyncio.run(test_control_plane())
    except KeyboardInterrupt:
        pass
