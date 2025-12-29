from server.strategies.base import CompositeStrategy
from .signal import CustomSignal
from .entry import CustomEntry
from .risk import CustomRisk
from .sizing import CustomSizing

class TopstepTest(CompositeStrategy):
    def __init__(self):
        super().__init__(
            name="TopstepTest",
            signal_module=CustomSignal("SignalLogic"),
            entry_module=CustomEntry("EntryLogic"),
            risk_module=CustomRisk("RiskLogic"),
            position_sizing_module=CustomSizing("SizingLogic")
        )