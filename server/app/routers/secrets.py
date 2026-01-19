from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from typing import Dict, Any, List

from ...security.secrets import KeyringBackend, KNOWN_INTEGRATIONS

router = APIRouter(prefix="/api/secrets", tags=["secrets"])

class SecretInput(BaseModel):
    key: str
    value: str

class SecretStatus(BaseModel):
    key: str
    label: str
    is_set: bool

@router.get("/list", response_model=List[SecretStatus])
async def list_secrets():
    """
    List all known integration keys and their set/unset status.
    Security: Returns status only, NEVER values.
    """
    kb = KeyringBackend()
    results = []
    
    # Reload keyring backend to ensure fresh state if needed, 
    # though instantiation is usually lightweight.
    
    for key, label in KNOWN_INTEGRATIONS:
        # Logic mirrors CLI: Check prefixed first, then raw
        full_key = key if key.startswith("QUANUX_") else f"QUANUX_{key}"
        
        val = kb.get(full_key)
        if not val and not key.startswith("QUANUX_"):
            val = kb.get(key)
            
        results.append(SecretStatus(
            key=key,
            label=label,
            is_set=bool(val)
        ))
    return results

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
