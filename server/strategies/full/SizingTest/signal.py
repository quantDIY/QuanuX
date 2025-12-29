from server.strategies.base import SignalModule, SignalType, BaseParameters

class CustomSignal(SignalModule):
    def define_parameters(self):
        class Params(BaseParameters):
            period: int = 14
        return Params

    def on_bar(self, bar_data):
        # Placeholder Logic
        return SignalType.NEUTRAL