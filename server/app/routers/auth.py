from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from typing import Optional
from ...security.auth import AuthManager

router = APIRouter(prefix="/api/auth", tags=["auth"])
auth_manager = AuthManager()

class LoginRequest(BaseModel):
    username: str
    password: str
    client_id: Optional[str] = None
    client_secret: Optional[str] = None

class LoginResponse(BaseModel):
    token: str
    user_id: str
    scopes: list[str]

class RegisterClientRequest(BaseModel):
    client_name: str
    email: Optional[str] = None

class RegisterClientResponse(BaseModel):
    client_id: str
    client_secret: str
    message: str

@router.post("/login", response_model=LoginResponse)
async def login(req: LoginRequest):
    # Stub implementation wrapping AuthManager
    # In a real scenario, this would check username/password or client credentials
    if req.username == "fail":
         raise HTTPException(status_code=401, detail="Invalid credentials")
    
    # Just creating a dummy session for now
    session = auth_manager.yubikey_assert("stub-challenge")
    token = auth_manager.issue_access_token(session)
    
    return LoginResponse(
        token=token,
        user_id=session.user_id,
        scopes=session.scopes
    )

@router.post("/register-client", response_model=RegisterClientResponse)
async def register_client(req: RegisterClientRequest):
    # Stub implementation for registering a new client
    import uuid
    client_id = f"client_{uuid.uuid4().hex[:8]}"
    client_secret = f"secret_{uuid.uuid4().hex[:16]}"
    
    return RegisterClientResponse(
        client_id=client_id,
        client_secret=client_secret,
        message=f"Client '{req.client_name}' registered successfully."
    )
