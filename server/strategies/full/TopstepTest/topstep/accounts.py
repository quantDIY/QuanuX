import httpx
import os
from typing import List, Any

# TODO: Move to config
BASE_API_URL = os.getenv("TOPSTEP_API_URL", "https://api.topstepx.com")

async def search_accounts(token: str, only_active: bool = True) -> List[Any]:
    url = f"{BASE_API_URL}/api/Account/search"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"onlyActiveAccounts": only_active}
    
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        data = response.json().get("accounts", [])
        if data:
            print(f"DEBUG: Account sample: {data[0]}")
        return data
        
    return []