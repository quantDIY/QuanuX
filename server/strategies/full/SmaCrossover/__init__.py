
from server.strategies.base import CompositeStrategy
from server.strategies.sizing.static import StaticSizing
from .signal import SmaSignal
from .entry import MarketEntry
from .risk import FixedPercentRisk

class SmaCrossoverStrategy(CompositeStrategy):
    def __init__(self):
        super().__init__(
            name="SmaCrossover",
            signal_module=SmaSignal(name="SmaSignal", params={"fast_period": 10, "slow_period": 20}),
            entry_module=MarketEntry(name="MarketEntry"),
            risk_module=FixedPercentRisk(name="FixedPercentRisk", params={"stop_loss_pct": 0.01, "take_profit_pct": 0.02}),
            position_sizing_module=StaticSizing(name="StaticOne", params={"quantity": 1.0})
        )
