from fastapi import FastAPI
from contextlib import asynccontextmanager
import asyncio
import os
import logging
from server.ha.sentinel import GlobalSentinelLoop

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger("quanux.main")

NODE_ID = os.getenv("QUANUX_NODE_ID", "local-dev-node")
NATS_URL = os.getenv("NATS_URL", "nats://localhost:4222")

# Instantiate the singleton Sentinel
sentinel = GlobalSentinelLoop(node_id=NODE_ID, nats_url=NATS_URL)

@asynccontextmanager
async def lifespan(app: FastAPI):
    """
    FastAPI Lifespan Context Manager.
    Anchors the GlobalSentinelLoop HA logic to the application lifecycle.
    """
    logger.info(f"Starting QuanuX Tier 1 Node: {NODE_ID}")
    
    # 1. Initialize NATS connection and Sentinel Background Tasks
    await sentinel.start()
    
    logger.info("Global Sentinel Loop Initialized. Resuming Ritchie FSM operations.")
    
    yield  # Application runs here and handles requests
    
    logger.info("Shutting down QuanuX Tier 1 Node. Releasing locks.")
    # Cleanly teardown NATS KV and Sentinel loops
    await sentinel.disconnect()

# The Backend-Driven Client Architecture Root
app = FastAPI(lifespan=lifespan, title="QuanuX Tier 1 API")

@app.get("/health")
async def health_check():
    """Returns local node runtime status alongside cluster state"""
    return {
        "status": "online",
        "node_id": NODE_ID,
        "is_leader": sentinel._is_leader
    }
