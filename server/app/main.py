from __future__ import annotations
import logging
from contextlib import asynccontextmanager
from fastapi import FastAPI
from .config import ServerConfig
from .logging import setup_logging
from .security.cors import add_cors
from .api.health import router as health_router
from .ws.endpoints import router as ws_router

@asynccontextmanager
async def lifespan(app: FastAPI):
    # startup
    logging.getLogger(__name__).info("Starting QuanuX Server (scaffold)…")
    yield
    # shutdown
    logging.getLogger(__name__).info("Stopping QuanuX Server (scaffold)…")

def create_app() -> FastAPI:
    setup_logging()
    cfg = ServerConfig()
    app = FastAPI(
        title=cfg.app_name,
        version=cfg.version,
        lifespan=lifespan,
    )
    add_cors(app, cfg.security.cors_allowed_origins)
    app.include_router(health_router)
    app.include_router(ws_router)
    return app

app = create_app()
