from server.strategies.base import RiskModule

class CustomRisk(RiskModule):
    def calculate_risk(self, entry_price, signal, bar_data):
        return {"stop_loss": 0.0, "take_profit": 0.0}