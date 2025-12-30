
from typing import Dict, Any, Optional, Type
from server.strategies.base import EntryModule, SignalType, BaseParameters
from server.app.domain.contracts import OrderRequest, Side, Type as OrderType, TimeInForce

class MarketEntry(EntryModule):
    def define_parameters(self) -> Type[BaseParameters]:
        return BaseParameters

    def generate_entry(self, signal: SignalType, bar_data: Any) -> Optional[Dict[str, Any]]:
        if signal == SignalType.NEUTRAL:
            return None
            
        side = Side.BUY if signal == SignalType.LONG else Side.SELL
        
        # Using the Generated Pydantic Model 'OrderRequest' structure conceptually,
        # but returning a dict for the Compositor/Engine to handle.
        return {
            "type": "MARKET", # Using string literal or enum value
            "side": side,
            "time_in_force": "DAY"
        }
