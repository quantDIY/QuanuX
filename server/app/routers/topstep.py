from fastapi import APIRouter, HTTPException, Depends
from ..domain.topstep.models import LoginRequest, LoginResponse, AccountSearchResponse
from ..domain.topstep.auth import authenticate
from ..domain.topstep.accounts import search_accounts

router = APIRouter(prefix="/topstep", tags=["Topstep"])

@router.post("/login", response_model=LoginResponse)
async def login(creds: LoginRequest):
    try:
        token = await authenticate(creds)
        return LoginResponse(token=token)
    except Exception as e:
        raise HTTPException(status_code=401, detail=str(e))

@router.get("/accounts", response_model=AccountSearchResponse)
async def get_accounts(token: str):
    # TODO: In a real app, token should come from a dependency (e.g. Bearer header)
    # For now, we accept it as a query param for simplicity in migration
    try:
        accounts = await search_accounts(token)
        return AccountSearchResponse(accounts=accounts)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

import httpx
from pydantic import BaseModel
from typing import List, Optional

class ConnectRequest(BaseModel):
    hub_url: str
    access_token: Optional[str] = None
    skip_negotiation: bool = False

class SubscribeRequest(BaseModel):
    method: str
    args: List[str] = []

BRIDGE_URL = "http://localhost:8077"

@router.post("/connect")
async def connect_signalr(req: ConnectRequest):
    async with httpx.AsyncClient() as client:
        try:
            resp = await client.post(f"{BRIDGE_URL}/connect", json=req.dict())
            resp.raise_for_status()
            return resp.json()
        except httpx.HTTPError as e:
            raise HTTPException(status_code=502, detail=f"Bridge connection failed: {e}")

@router.post("/subscribe")
async def subscribe_signalr(req: SubscribeRequest):
    async with httpx.AsyncClient() as client:
        try:
            resp = await client.post(f"{BRIDGE_URL}/subscribe", json=req.dict())
            resp.raise_for_status()
            return resp.json()
        except httpx.HTTPError as e:
            raise HTTPException(status_code=502, detail=f"Bridge subscription failed: {e}")
