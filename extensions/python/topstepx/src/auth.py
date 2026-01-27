import httpx
import os
from models import LoginRequest, LoginResponse

# TODO: Move to config
DEFAULT_API_URL = os.getenv("QUANUX_TOPSTEP__BASE_API_URL", "https://api.topstepx.com")

async def authenticate(creds: LoginRequest, base_url: str = DEFAULT_API_URL) -> str:
    url = f"{base_url}/api/Auth/loginKey"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json"
    }
    payload = {
        "userName": creds.username,
        "password": creds.password,
        "apiKey": creds.api_key
    }
    
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        data = response.json()
        token = data.get("token")
        if token:
            return token
            
    raise Exception(f"Authentication failed: {response.text}")
