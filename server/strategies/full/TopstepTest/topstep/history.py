import httpx
import os
from datetime import datetime, timedelta
from typing import Any, Dict, Optional

# TODO: Move to config
BASE_API_URL = os.getenv("TOPSTEP_API_URL", "https://api.topstepx.com")

async def retrieve_bars(
    token: str, 
    contract_id: int, 
    minutes: int = 100, 
    live: bool = False
) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/History/retrieveBars"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }

    from datetime import timezone
    end_time = datetime.now(timezone.utc)
    start_time = end_time - timedelta(minutes=minutes)

    payload = {
        "contractId": contract_id,
        "live": live,
        "startTime": start_time.isoformat() + "Z",
        "endTime": end_time.isoformat() + "Z",
        "unit": 2,  # Minute
        "unitNumber": 1,
        "limit": minutes,
        "includePartialBar": False
    }
    
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return response.json()
        
    return {}