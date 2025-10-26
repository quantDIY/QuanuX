"""
QuanuX Secrets Broker (scaffold) — v0.0.1

Unified façade for secrets access. No plaintext at rest.
Backends:
  - OS Keyring (preferred on desktop)
  - Vault (server-grade)
  - Redis (cache-only; NEVER source of truth)
"""

from __future__ import annotations
from dataclasses import dataclass
from typing import Optional, Protocol

class SecretsBackend(Protocol):
    def get(self, key: str) -> Optional[str]: ...
    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None: ...

@dataclass
class SecretsBroker:
    primary: SecretsBackend
    fallback: list[SecretsBackend]

    def get(self, key: str) -> Optional[str]:
        val = self.primary.get(key)
        if val is not None:
            return val
        for b in self.fallback:
            val = b.get(key)
            if val is not None:
                return val
        return None

    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None:
        self.primary.set(key, value, ttl_seconds=ttl_seconds)
        for b in self.fallback:
            try:
                b.set(key, value, ttl_seconds=ttl_seconds)
            except Exception:
                # best-effort cache; ignore
                pass

# --- Backends (stubs) ---
class KeyringBackend:
    """OS Keyring placeholder. Actual impl will call `keyring` lib."""
    def get(self, key: str) -> Optional[str]: return None
    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None: ...

class VaultBackend:
    """Vault placeholder. Actual impl will use hvac or REST."""
    def __init__(self, url: str | None = None): self.url = url
    def get(self, key: str) -> Optional[str]: return None
    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None: ...

class RedisCacheBackend:
    """Redis cache-only placeholder. DO NOT rely on as source of truth."""
    def __init__(self, url: str | None = None): self.url = url
    def get(self, key: str) -> Optional[str]: return None
    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None: ...
