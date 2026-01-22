
from fastapi import APIRouter, HTTPException, Body
from pydantic import BaseModel
from typing import Optional
import logging

# Import the bridge
# Assuming server package structure allows this import
from server.bridges.rithmic_bridge import RithmicBridge

router = APIRouter(prefix="/rithmic", tags=["rithmic"])
logger = logging.getLogger(__name__)

# Global singleton for the bridge (simple approach for now)
# In production, might want to attach to app.state or use dependency injection
# initialized lazily or on startup
_BRIDGE: Optional[RithmicBridge] = None

class ConnectRequest(BaseModel):
    user: str
    password: str
    server: str = "Rithmic Paper Trading"
    app_name: str = "QuanuX"
    app_version: str = "1.0.0"

class SubscribeRequest(BaseModel):
    exchange: str
    ticker: str

class OrderRequest(BaseModel):
    exchange: str
    ticker: str
    qty: int
    price: float
    is_buy: bool

class ModifyOrderRequest(BaseModel):
    exchange: str
    ticker: str
    order_num: str
    qty: int
    price: float

def get_bridge() -> RithmicBridge:
    global _BRIDGE
    if _BRIDGE is None:
        raise HTTPException(status_code=503, detail="Rithmic Bridge not initialized (Connect first)")
    return _BRIDGE

@router.post("/connect")
def connect(req: ConnectRequest):
    global _BRIDGE
    try:
        if _BRIDGE is None:
            _BRIDGE = RithmicBridge(req.user, req.password, req.server, req.app_name, req.app_version)
        
        # In current bridge impl, params are set in constructor, connect calls login
        # If re-connecting with new params, we might need a new instance or update params
        if _BRIDGE.login_params.sMdUser != req.user:
             _BRIDGE = RithmicBridge(req.user, req.password, req.server, req.app_name, req.app_version)

        _BRIDGE.connect()
        return {"status": "connected", "user": req.user}
    except Exception as e:
        logger.error(f"Connection failed: {e}")
        raise HTTPException(status_code=500, detail=str(e))

@router.post("/disconnect")
def disconnect():
    global _BRIDGE
    if _BRIDGE:
        _BRIDGE.disconnect()
        _BRIDGE = None
    return {"status": "disconnected"}

@router.post("/subscribe")
def subscribe(req: SubscribeRequest):
    bridge = get_bridge()
    try:
        bridge.subscribe_market_data(req.exchange, req.ticker)
        return {"status": "subscribed", "ticker": req.ticker}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@router.post("/orders/send")
def send_order(req: OrderRequest):
    bridge = get_bridge()
    try:
        bridge.send_limit_order(req.exchange, req.ticker, req.qty, req.price, req.is_buy)
        return {"status": "sent", "order": req.dict()}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@router.post("/orders/modify")
def modify_order(req: ModifyOrderRequest):
    bridge = get_bridge()
    try:
        bridge.modify_order(req.exchange, req.ticker, req.order_num, req.qty, req.price)
        return {"status": "modified", "order_num": req.order_num}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

