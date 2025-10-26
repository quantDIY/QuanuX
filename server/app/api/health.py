from fastapi import APIRouter
from ..config import ServerConfig

router = APIRouter(tags=["health"])

@router.get("/health")
async def health():
    return {"ok": True}

@router.get("/ready")
async def ready():
    # later: check bus, db, cache, adapters
    return {"ready": True, "deps": {"event_bus": "memory"}}

@router.get("/version")
async def version():
    cfg = ServerConfig()  # loads from env
    return {"app": cfg.app_name, "version": cfg.version}

@router.get("/config/snapshot")
async def config_snapshot():
    cfg = ServerConfig()
    return cfg.redacted()
