from pydantic import BaseModel, Field
from typing import Optional, List

class RithmicCredentials(BaseModel):
    username: str
    password: str
    system_name: str = Field(default="Rithmic Paper Trading", description="e.g. Rithmic Paper Trading, TopstepTrader")
    server_region: str = Field(default="Chicago", description="e.g. Chicago, Frankfurt, Singapore")

class RithmicAccount(BaseModel):
    account_id: str
    name: str
    currency: str
    balance: float

class RithmicOrder(BaseModel):
    order_id: str
    symbol: str
    side: str # BUY / SELL
    quantity: int
    price: Optional[float]
    status: str
