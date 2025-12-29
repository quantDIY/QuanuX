from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from typing import Dict, Any

from ...security.secrets import KeyringBackend

router = APIRouter(prefix="/api/secrets", tags=["secrets"])

class SecretInput(BaseModel):
    key: str
    value: str

@router.post("")
async def store_secret(input: SecretInput):
    """
    Store a secret in the OS Keyring.
    The key will be prefixed with QUANUX_ if not already present, 
    matching the Pydantic BaseSettings convention.
    """
    kb = KeyringBackend()
    if not kb._keyring:
        raise HTTPException(status_code=500, detail="Keyring backend unavailable on server.")

    # Standardize key (Pydantic BaseSettings usually looks for QUANUX_ prefix)
    key = input.key.upper()
    if not key.startswith("QUANUX_"):
        key = f"QUANUX_{key}"

    try:
        kb.set(key, input.value)
        return {"status": "success", "key": key}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
