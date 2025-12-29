from server.strategies.base import CompositeStrategy
from .signal import CustomSignal
from .entry import CustomEntry
from .risk import CustomRisk

class Test_Strategy(CompositeStrategy):
    def __init__(self):
        super().__init__(
            name="Test_Strategy",
            signal_module=CustomSignal("SignalLogic"),
            entry_module=CustomEntry("EntryLogic"),
            risk_module=CustomRisk("RiskLogic")
        )