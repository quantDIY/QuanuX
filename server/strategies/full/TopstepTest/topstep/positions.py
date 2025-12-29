import httpx
import os
from typing import Any, Dict

# TODO: Move to config
BASE_API_URL = os.getenv("TOPSTEP_API_URL", "https://api.topstepx.com")

async def search_open_positions(token: str, account_id: int) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Position/searchOpen"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"accountId": account_id}
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    return {"success": False, "error": response.text}

async def close_position(token: str, account_id: int, contract_id: int) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Position/close"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"accountId": account_id, "contractId": contract_id}
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    # Return error code if available for testing
    try:
        error_data = response.json()
        return {"success": False, "errorCode": error_data.get("errorCode"), "error": response.text, "status": response.status_code}
    except:
        return {"success": False, "error": response.text, "status": response.status_code}

async def partial_close_position(token: str, account_id: int, contract_id: int, size: int) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Position/partialClose"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"accountId": account_id, "contractId": contract_id, "size": size}
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    try:
        error_data = response.json()
        return {"success": False, "errorCode": error_data.get("errorCode"), "error": response.text, "status": response.status_code}
    except:
        return {"success": False, "error": response.text, "status": response.status_code}