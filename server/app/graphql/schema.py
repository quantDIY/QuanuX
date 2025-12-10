import strawberry
import asyncio
from typing import AsyncGenerator, List, Optional
from ..domain.topstep.accounts import search_accounts
from ..domain.topstep.contracts import search_contracts
from ..config import ServerConfig

@strawberry.type
class TopstepAccount:
    id: str
    name: str
    balance: float

@strawberry.type
class TopstepContract:
    id: str
    contractName: str
    contractLabel: str
    tickSize: float

@strawberry.type
class Query:
    @strawberry.field
    def hello(self) -> str:
        return "Hello from QuanuX GraphQL!"

    @strawberry.field
    async def active_accounts(self) -> List[TopstepAccount]:
        config = ServerConfig()
        token = config.topstep.session_token
        if not token:
            raise Exception("No session token found. Please run 'python -m server.cli.refresh_token'.")
            
        accounts_data = await search_accounts(token, only_active=True)
        return [
            TopstepAccount(
                id=str(a["id"]),
                name=a["name"],
                balance=float(a.get("balance", 0.0))
            ) for a in accounts_data
        ]

    @strawberry.field
    async def map_contracts(self, search_text: str = "NQ") -> List[TopstepContract]:
        config = ServerConfig()
        token = config.topstep.session_token
        if not token:
            raise Exception("No session token found. Please run 'python -m server.cli.refresh_token'.")

        contracts_data = await search_contracts(token, search_text=search_text)
        return [
            TopstepContract(
                id=str(c["id"]),
                contractName=c["name"],
                contractLabel=c["description"],
                tickSize=c["tickSize"]
            ) for c in contracts_data
        ]

@strawberry.type
class Mutation:
    @strawberry.mutation
    def echo(self, message: str) -> str:
        return f"Echo: {message}"

@strawberry.type
class Subscription:
    @strawberry.subscription
    async def count(self, target: int = 10) -> AsyncGenerator[int, None]:
        for i in range(target):
            yield i
            await asyncio.sleep(1)

schema = strawberry.Schema(query=Query, mutation=Mutation, subscription=Subscription)
