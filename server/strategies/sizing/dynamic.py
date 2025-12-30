
from typing import Type
from server.strategies.base import PositionSizingModule, SignalType, BaseParameters

class DynamicRiskSizing(PositionSizingModule):
    """
    Calculates position size based on a percentage of account equity at risk.
    Requires a stop_loss distance (not passed here yet, so this is a simplified equity % model).
    
    NOTE: Real dynamic risk sizing usually requires knowledge of the Stop Loss distance 
    to calculate (Risk Amount / Stop Distance). Since calculate_size matches the base signature,
    we currently implement a 'Percent of Equity Value' or simple leverage calculation.
    
    If we want 'Risk % (e.g. 1% risk per trade)', we need to know the Stop Loss. 
    Refactoring base.py might be needed for true Risk Sizing, or we assume a fixed stop shim.
    
    For now, this implements: "Target Position Value = Equity * X%"
    """
    def define_parameters(self) -> Type[BaseParameters]:
        class Params(BaseParameters):
            risk_percent: float = 0.02  # 2% of equity per trade value
        return Params

    def calculate_size(self, signal: SignalType, price: float, account_equity: float) -> float:
        if price <= 0:
            return 0.0
            
        risk_pct = self.params.get("risk_percent", 0.02)
        target_value = account_equity * risk_pct
        
        # contracts/shares = Target Value / Price
        return target_value / price
