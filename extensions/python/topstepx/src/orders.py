import httpx
import os
from typing import Any, Dict, Optional

# TODO: Move to config
BASE_API_URL = os.getenv("QUANUX_TOPSTEP__BASE_API_URL", "https://api.topstepx.com")

async def search_orders(token: str, account_id: int, start_time: str, end_time: str) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Order/search"
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

async def search_open_orders(token: str, account_id: int) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Order/searchOpen"
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

async def place_order(token: str, order_data: Dict[str, Any]) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Order/place"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=order_data, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    return {"success": False, "error": response.text}

async def cancel_order(token: str, account_id: int, order_id: int) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Order/cancel"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"accountId": account_id, "orderId": order_id}
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    return {"success": False, "error": response.text}

async def modify_order(token: str, account_id: int, order_id: int, **kwargs) -> Dict[str, Any]:
    url = f"{BASE_API_URL}/api/Order/modify"
    headers = {
        "accept": "text/plain",
        "Content-Type": "application/json",
        "Authorization": f"Bearer {token}"
    }
    payload = {"accountId": account_id, "orderId": order_id, **kwargs}
    async with httpx.AsyncClient() as client:
        response = await client.post(url, json=payload, headers=headers)
        
    if response.is_success:
        return {"success": True, **response.json()}
    return {"success": False, "error": response.text}
