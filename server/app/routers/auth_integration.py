from fastapi import APIRouter, HTTPException, Request
from fastapi.responses import RedirectResponse
import uuid
from ...security.secrets import KeyringBackend

router = APIRouter(prefix="/api/auth", tags=["integration-auth"])

@router.get("/{provider}/start")
async def start_auth_flow(provider: str):
    """
    Initiates the OAuth flow for a given provider.
    In a real app, this would redirect to the provider's authorization page.
    """
    provider = provider.lower()
    if provider not in ["topstep", "google"]:
        raise HTTPException(status_code=400, detail="Unsupported provider (OpenAI requires manual API key entry)")

    # MOCK: In dev, we just redirect straight to our callback with a fake code
    # Real World: return RedirectResponse("https://provider.com/oauth/authorize?...")
    
    fake_code = f"auth_code_{uuid.uuid4().hex[:8]}"
    callback_url = f"/api/auth/{provider}/callback?code={fake_code}"
    
    # We redirect the browser to our own callback to simulate the round-trip
    return RedirectResponse(url=callback_url)

@router.get("/{provider}/callback")
async def auth_callback(provider: str, code: str):
    """
    Handles the callback from the provider.
    Exchanges the code for a token and stores it in the keyring.
    """
    provider = provider.lower()
    
    # MOCK: Token Exchange
    # access_token = exchange_code_for_token(code)
    access_token = f"mock_token_{provider}_{uuid.uuid4().hex[:16]}"
    
    # Store in Keyring
    kb = KeyringBackend()
    if kb._keyring:
        key = f"QUANUX_{provider.upper()}_ACCESS_TOKEN"
        kb.set(key, access_token)
    
    # Redirect back to the Integrations page with a success flag
    return RedirectResponse(url="http://localhost:5173/integrations?success=true&provider=" + provider)
