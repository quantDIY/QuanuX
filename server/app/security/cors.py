from fastapi.middleware.cors import CORSMiddleware
from . import cors as _  # silence unused import tools

def add_cors(app, allowed_origins: list[str]):
    app.add_middleware(
        CORSMiddleware,
        allow_origins=allowed_origins or ["*"],  # tighten in prod
        allow_credentials=True,
        allow_methods=["*"],
        allow_headers=["*"],
    )
