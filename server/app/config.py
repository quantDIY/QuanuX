from __future__ import annotations
from pydantic import BaseModel, Field
from pydantic_settings import BaseSettings
from typing import List

REDACT = "🔒REDACTED"

class SecurityConfig(BaseModel):
    auth_mode: str = "webauthn|yubikey|oauth2"
    mtls_enabled: bool = False
    cors_allowed_origins: List[str] = Field(default_factory=lambda: ["http://localhost:5173", "http://localhost:5174", "tauri://localhost", "http://127.0.0.1:5173", "http://127.0.0.1:5174"])
    
    # MFA
    mfa_enabled: bool = False
    mfa_allowed_methods: List[str] = Field(default_factory=lambda: ["totp", "webauthn"])

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
    
    class TopstepConfig(BaseModel):
        username: str | None = None
        password: str | None = None
        api_key: str | None = None
        session_token: str | None = None
        base_api_url: str = "https://api.topstepx.com"
        market_hub_url: str | None = None
        
    topstep: TopstepConfig = TopstepConfig()

    # Example third-party keys (NEVER persisted here; just to show redaction)
    tradingview_client_id: str | None = None
    broker_api_key: str | None = None

    class Config:
        env_prefix = "QUANUX_"
        env_nested_delimiter = "__"  # e.g. QUANUX__SECURITY__MTLS_ENABLED=true

    @classmethod
    def settings_customise_sources(
        cls,
        settings_cls: type[BaseSettings],
        init_settings: PydanticBaseSettingsSource,
        env_settings: PydanticBaseSettingsSource,
        dotenv_settings: PydanticBaseSettingsSource,
        file_secret_settings: PydanticBaseSettingsSource,
    ) -> tuple[PydanticBaseSettingsSource, ...]:
        return (
            init_settings,
            env_settings,
            KeyringSettingsSource(settings_cls),
            dotenv_settings,
            file_secret_settings,
        )

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

from pydantic_settings import PydanticBaseSettingsSource
from typing import Any, Dict, Tuple

class KeyringSettingsSource(PydanticBaseSettingsSource):
    """
    A custom Pydantic settings source that reads from the OS Keyring.
    It expects keys to be prefixed with the env_prefix (e.g. QUANUX_).
    """
    def get_field_value(
        self, field: Field, field_name: str
    ) -> Tuple[Any, str, bool]:
        # This method is not used directly in newer Pydantic versions for full dict construction
        # but required by abstract base class in some versions.
        # We implement __call__ instead for full control.
        return None, field_name, False

    def __call__(self) -> Dict[str, Any]:
        from ..security.secrets import KeyringBackend
        kb = KeyringBackend()
        if not kb._keyring:
            return {}
        
        d: Dict[str, Any] = {}
        
        # We iterate over the fields to find what to look for
        # This is a simplified implementation that constructs keys based on prefix
        prefix = self.config.get("env_prefix", "QUANUX_")
        delimiter = self.config.get("env_nested_delimiter", "__")
        
        # print(f"DEBUG: Checking Keyring with prefix={prefix}")

        for field_name, field in self.settings_cls.model_fields.items():
            # Construct the key name expected in Keyring (same as Env var)
            # e.g. topstep -> QUANUX__TOPSTEP
            # But wait, nested models are tricky.
            # For top-level fields:
            key = f"{prefix}{field_name.upper()}"
            val = kb.get(key)
            if val is not None:
                d[field_name] = val
            
            # For nested models (like TopstepConfig), we need to handle them.
            # Pydantic's EnvSettingsSource handles this complexity.
            # For simplicity in this scaffold, we'll manually check known nested configs
            # or rely on the user setting "QUANUX__TOPSTEP__API_KEY" and we parse it?
            # Actually, Pydantic expects a dict structure.
            
            # Let's try to support the specific nested fields we care about for now.
            if field_name == "topstep":
                # Look for QUANUX__TOPSTEP__USERNAME, QUANUX__TOPSTEP__API_KEY
                ts_d = {}
                for sub_field in ["username", "password", "api_key", "session_token", "market_hub_url"]:
                    sub_key = f"{prefix}TOPSTEP{delimiter}{sub_field.upper()}"
                    sub_val = kb.get(sub_key)
                    print(f"DEBUG: Checking {sub_key} -> {sub_val is not None}")
                    if sub_val:
                        ts_d[sub_field] = sub_val
                if ts_d:
                    d["topstep"] = ts_d

        return d
