import httpx
import os
from typing import Any, Dict

# TODO: Move to config
BASE_API_URL = os.getenv("QUANUX_TOPSTEP__BASE_API_URL", "https://api.topstepx.com")

async def search_trades(token: str, account_id: int, start_time: str, end_time: str) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Trade/search"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {
        "accountId": account_id,
        "startTimestamp": start_time,
        "endTimestamp": end_time
    }
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    return {"success": False, "error": response.text}
