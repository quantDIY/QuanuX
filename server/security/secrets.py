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
    """OS Keyring implementation using `keyring` lib."""
    def __init__(self, service_name: str = "QuanuX"):
        self.service_name = service_name
        self._keyring = None
        try:
            import keyring
            # Basic sanity check to ensure we can access a backend
            # This helps catch cases where it imports but fails to find a backend
            keyring.get_keyring()
            self._keyring = keyring
        except ImportError:
             print("KeyringBackend: 'keyring' library not found.")
        except Exception as e:
             import traceback
             print(f"KeyringBackend: Failed to initialize keyring: {e}")
             # traceback.print_exc() # Optional: verbose debugging

    def get(self, key: str) -> Optional[str]:
        if not self._keyring:
            return None
        try:
            return self._keyring.get_password(self.service_name, key)
        except Exception as e:
            print(f"KeyringBackend.get error: {e}")
            return None

    def set(self, key: str, value: str, *, ttl_seconds: int | None = None) -> None:
        if not self._keyring:
            raise RuntimeError("Keyring is not available on this system.")
        try:
            self._keyring.set_password(self.service_name, key, value)
        except Exception as e:
            print(f"KeyringBackend.set error: {e}")
            raise e

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
