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
class MarketMessage:
    symbol: str
    price: float
    ts: float

@strawberry.type
class Subscription:
    @strawberry.subscription
    async def count(self, target: int = 10) -> AsyncGenerator[int, None]:
        for i in range(target):
            yield i
            await asyncio.sleep(1)

    @strawberry.subscription
    async def market_data(self, symbol: str) -> AsyncGenerator[MarketMessage, None]:
        from ..services.nats import NatsService
        nats = NatsService()
        
        # Subscribe to market.{symbol}
        iterator = nats.subscribe_iterator(f"market.{symbol}")
        
        async for msg in iterator:
            # Assume msg.data is JSON: {"price": 100.0, "ts": ...}
            try:
                import json
                data = json.loads(msg.data.decode())
                yield MarketMessage(
                    symbol=symbol,
                    price=float(data.get("price", 0.0)),
                    ts=float(data.get("ts", 0.0))
                )
            except Exception:
                continue

schema = strawberry.Schema(query=Query, mutation=Mutation, subscription=Subscription)
