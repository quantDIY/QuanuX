import uuid
import json
import logging
from fastapi import APIRouter, Request, BackgroundTasks
from pydantic import BaseModel
from typing import Optional

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/foundry", tags=["Foundry"])

class FoundryForgeRequest(BaseModel):
    component_type: str  # indicator, entry, exit, strategy
    name: str
    target_lang: str     # python, cython, cpp
    target_version: Optional[str] = None
    prompt: Optional[str] = None
    
@router.post("/forge")
async def start_forge(req: FoundryForgeRequest, request: Request, background_tasks: BackgroundTasks):
    """Initiates an AI generation payload via NATS."""
    job_id = f"job_{uuid.uuid4().hex[:8]}"
    nats = request.app.state.nats
    
    payload = {
        "job_id": job_id,
        "component_type": req.component_type,
        "name": req.name,
        "target_lang": req.target_lang,
        "target_version": req.target_version,
        "prompt": req.prompt
    }
    
    await nats.publish("sys.foundry.request.forge", json.dumps(payload).encode('utf-8'))
    logger.info(f"Published forge request: {job_id}")
    
    return {"status": "accepted", "job_id": job_id}

class FoundryVerifyRequest(BaseModel):
    strategy_name: str
    
@router.post("/verify")
async def start_verify(req: FoundryVerifyRequest, request: Request, background_tasks: BackgroundTasks):
    """Triggers the deterministic sandbox for mathematical equivalence testing."""
    job_id = f"job_{uuid.uuid4().hex[:8]}"
    nats = request.app.state.nats
    
    payload = {
        "job_id": job_id,
        "strategy_name": req.strategy_name
    }
    
    await nats.publish("sys.foundry.request.verify", json.dumps(payload).encode('utf-8'))
    logger.info(f"Published verify request: {job_id}")
    
    return {"status": "accepted", "job_id": job_id}
