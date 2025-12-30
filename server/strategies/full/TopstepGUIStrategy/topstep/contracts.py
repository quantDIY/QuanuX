import httpx
import os
from typing import List, Any, Optional

# TODO: Move to config
BASE_API_URL = os.getenv("TOPSTEP_API_URL", "https://api.topstepx.com")

async def search_contracts(token: str, search_text: str = "NQ", live: bool = False) -> List[Any]:
    url = f"{BASE_API_URL}/api/Contract/search"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"searchText": search_text, "live": live}
    
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        data = response.json().get("contracts", [])
        if data:
            print(f"DEBUG: Contract sample: {data[0]}")
        return data
        
    return []

async def search_contract_by_id(token: str, contract_id: int) -> Optional[Any]:
    url = f"{BASE_API_URL}/api/Contract/searchById"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"contractId": contract_id}
    
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return response.json()
        
    return None
