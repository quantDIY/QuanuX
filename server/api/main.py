from fastapi import FastAPI, BackgroundTasks, APIRouter, HTTPException, Request
from fastapi.responses import StreamingResponse
from pydantic import BaseModel
import asyncio
import socket
import logging
import json
import os
from contextlib import asynccontextmanager

import subprocess

class ProbeResult(BaseModel):
    status: str
    details: str

LOG_FILE = "/tmp/quanux_fix.log"

def execute_probe(fix=False, log_to_console=False):
    try:
        res = subprocess.run(["qxctl", "probe"], capture_output=True, text=True)
        return ProbeResult(status="healthy" if res.returncode == 0 else "degraded", details="Native Go CLI probe execution successful.")
    except Exception as e:
        return ProbeResult(status="error", details=str(e))

# Ensure nats-py is available
try:
    import nats
    from nats.errors import ConnectionClosedError, TimeoutError, NoServersError
    NATS_AVAILABLE = True
except ImportError:
    NATS_AVAILABLE = False

logger = logging.getLogger("quanux.api")
nc = None  # Global NATS client

async def watchdog_listener(nc_client):
    """Background task listening for the remote Nudge."""
    hostname = socket.gethostname()
    subject = f"QUANUX.NODE.PROBE.{hostname}"
    
    if not nc_client:
        logger.warning("Watchdog listener aborted: NATS client absent.")
        return
        
    async def message_handler(msg):
        logger.info("📡 Received remote Nudge! Executing self-check (Fix=False)...")
        result = execute_probe(fix=False, log_to_console=False)
        reply_data = json.dumps(result.dict()).encode()
        
        try:
            if msg.reply:
                await nc_client.publish(msg.reply, reply_data)
            await nc_client.publish(f"QUANUX.NODE.STATUS.{hostname}", reply_data)
        except Exception as e:
            logger.error(f"Watchdog failed to reply: {e}")

    try:
        await nc_client.subscribe(subject, cb=message_handler)
        logger.info(f"Watchdog Receptor listening on {subject}")
    except Exception as e:
        logger.error(f"Failed to subscribe to Watchdog: {e}")


async def setup_nats():
    """Isolated background task to initialize NATS and the Watchdog."""
    global nc
    try:
        # We allow indefinite reconnect attempts (-1) so it comes back when NATS starts,
        # but because this is a background task, it won't block the FastAPI HTTP server from binding!
        nc = await nats.connect("nats://127.0.0.1:4222", max_reconnect_attempts=-1, reconnect_time_wait=2)
        logger.info("Fast-Nerve connected to local NATS.")
        asyncio.create_task(watchdog_listener(nc))
    except Exception as e:
        logger.error(f"Fast-Nerve failed to connect to NATS: {e}")

@asynccontextmanager
async def lifespan(app: FastAPI):
    # Startup
    if NATS_AVAILABLE:
        # Spawn NATS setup separately to unblock HTTP initialization
        asyncio.create_task(setup_nats())
    else:
        logger.warning("Fast-Nerve running without nats-py. Watchdog unavailable.")
        
    yield
    
    # Shutdown
    if nc and not nc.is_closed:
        await nc.close()
        logger.info("Fast-Nerve NATS connection closed.")

app = FastAPI(title="QuanuX Fast-Nerve", lifespan=lifespan)

# --- Routes ---

@app.get("/probe", response_model=ProbeResult)
async def get_probe():
    """Execute a passive Neural Tap."""
    return execute_probe(fix=False, log_to_console=False)

@app.post("/nudge", response_model=ProbeResult)
async def post_nudge():
    """Manually trigger the Auto-Suture cycle."""
    logger.warning("Manual Nudge (Auto-Suture) triggered via API.")
    return execute_probe(fix=True, log_to_console=False)

@app.get("/logs")
async def get_logs(lines: int = 100):
    """Stream the tail of the Black Box fix.log."""
    if not os.path.exists(LOG_FILE):
        raise HTTPException(status_code=404, detail="Log file not found.")
        
    def log_generator():
        # A simple tail implementation. For production, `tail -n` subprocess is robust.
        # But reading in python is safer.
        with open(LOG_FILE, 'r') as f:
            all_lines = f.readlines()
            for line in all_lines[-lines:]:
                yield line
                
    return StreamingResponse(log_generator(), media_type="text/plain")

class LoggingConfig(BaseModel):
    max_bytes: int
    backup_count: int

@app.put("/config/logging")
async def update_logging_config(config: LoggingConfig):
    """Dynamically update the RotatingFileHandler (Black Box)."""
    probe_logger = logging.getLogger("quanux.probe")
    
    # We update the existing handler
    updated = False
    for handler in probe_logger.handlers:
        if isinstance(handler, logging.handlers.RotatingFileHandler):
            handler.maxBytes = config.max_bytes
            handler.backupCount = config.backup_count
            updated = True
            
    if updated:
        # Also write out /etc/quanux/logging.conf for persistence, simplified here.
        try:
            os.makedirs("/etc/quanux", exist_ok=True)
            with open("/etc/quanux/logging.conf", "w") as f:
                json.dump(config.dict(), f)
            return {"status": "success", "message": "Logging parameters updated dynamically."}
        except PermissionError:
            return {"status": "partial", "message": "Updated in memory, but missing permissions for /etc/quanux/logging.conf"}
            
    raise HTTPException(status_code=400, detail="RotatingFileHandler not found on probe logger.")

if __name__ == "__main__":
    import uvicorn
    uvicorn.run("server.api.main:app", host="0.0.0.0", port=8080, log_level="info")
