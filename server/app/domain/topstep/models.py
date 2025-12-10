from pydantic import BaseModel

class LoginRequest(BaseModel):
    username: str
    api_key: str

class LoginResponse(BaseModel):
    token: str

class Account(BaseModel):
    id: int
    name: str
    accountType: str
    # Add other fields as needed based on the API response
    
class AccountSearchResponse(BaseModel):
    accounts: list[Account]

class Contract(BaseModel):
    id: int
    contractName: str
    contractLabel: str
    tickSize: float
    # Add other fields as needed

class ContractSearchResponse(BaseModel):
    contracts: list[Contract]

