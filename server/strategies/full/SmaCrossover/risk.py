
from typing import Dict, Any, Type
from server.strategies.base import RiskModule, SignalType, BaseParameters

class FixedPercentRisk(RiskModule):
    def define_parameters(self) -> Type[BaseParameters]:
        class Params(BaseParameters):
            stop_loss_pct: float = 0.01
            take_profit_pct: float = 0.02
        return Params

    def calculate_risk(self, entry_price: float, signal: SignalType, bar_data: Any) -> Dict[str, float]:
        sl_pct = self.params['stop_loss_pct']
        tp_pct = self.params['take_profit_pct']
        
        if signal == SignalType.LONG:
            stop_loss = entry_price * (1 - sl_pct)
            take_profit = entry_price * (1 + tp_pct)
        else: # SHORT
            stop_loss = entry_price * (1 + sl_pct)
            take_profit = entry_price * (1 - tp_pct)
            
        return {
            "stop_loss": stop_loss,
            "take_profit": take_profit
        }
