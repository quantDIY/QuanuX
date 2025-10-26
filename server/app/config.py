from __future__ import annotations
from pydantic import BaseModel, Field
from pydantic_settings import BaseSettings
from typing import List

REDACT = "🔒REDACTED"

class SecurityConfig(BaseModel):
    auth_mode: str = "webauthn|yubikey|oauth2"
    mtls_enabled: bool = False
    cors_allowed_origins: List[str] = Field(default_factory=lambda: ["http://localhost:5173", "tauri://localhost"])

class ServerConfig(BaseSettings):
    app_name: str = "QuanuX Server"
    version: str = "0.0.1"
    env: str = "dev"
    bind_host: str = "0.0.0.0"
    bind_port: int = 8080

    # Event bus placeholder
    event_bus: str = "memory"  # "nats" later

    # Security
    security: SecurityConfig = SecurityConfig()

    # Example third-party keys (NEVER persisted here; just to show redaction)
    tradingview_client_id: str | None = None
    broker_api_key: str | None = None

    class Config:
        env_prefix = "QUANUX_"
        env_nested_delimiter = "__"  # e.g. QUANUX__SECURITY__MTLS_ENABLED=true

    def redacted(self) -> dict:
        d = self.model_dump()
        # redact any obvious secrets
        for k in list(d.keys()):
            if "key" in k.lower() or "secret" in k.lower() or "token" in k.lower():
                d[k] = REDACT
        for k, v in d.items():
            if isinstance(v, dict):
                for sk in list(v.keys()):
                    if "key" in sk.lower() or "secret" in sk.lower() or "token" in sk.lower():
                        v[sk] = REDACT
        return d
