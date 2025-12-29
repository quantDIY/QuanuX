from server.strategies.base import PositionSizingModule, SignalType

class CustomSizing(PositionSizingModule):
    def define_parameters(self):
        return None

    def calculate_size(self, signal, price, account_equity):
        return 1.0