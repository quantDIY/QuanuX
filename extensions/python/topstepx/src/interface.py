import os
import sys
import asyncio
import logging

# Set up logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("quanux-topstepx")

async def main():
    logger.info("TopstepX Extension Starting...")
    # TODO: Implement the persistent connection or command loop here
    # This is a placeholder to ensure the extension has an entry point
    while True:
        await asyncio.sleep(60)

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        pass
