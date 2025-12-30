
from typing import Type
from server.strategies.base import PositionSizingModule, SignalType, BaseParameters

class StaticSizing(PositionSizingModule):
    """
    Returns a fixed quantity for every trade.
    """
    def define_parameters(self) -> Type[BaseParameters]:
        class Params(BaseParameters):
            quantity: float = 1.0
        return Params

    def calculate_size(self, signal: SignalType, price: float, account_equity: float) -> float:
        return self.params.get("quantity", 1.0)
