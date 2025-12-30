
from server.strategies.base import SignalModule, SignalType, BaseParameters
from typing import Type
import pandas as pd

class SmaSignal(SignalModule):
    """
    Simple Moving Average Crossover Signal.
    Long when Fast SMA > Slow SMA.
    Short when Fast SMA < Slow SMA.
    """
    def define_parameters(self) -> Type[BaseParameters]:
        class Params(BaseParameters):
            fast_period: int = 10
            slow_period: int = 20
        return Params

    def on_bar(self, bar_data: dict) -> SignalType:
        # Stateful method. Assumes sequential calls.
        if not hasattr(self, '_history'):
            self._history = []
        
        # Expect normalized 'close' key
        close = bar_data.get('close')
        if close is None:
            # Fallback for robustness? Or strict error?
            return SignalType.NEUTRAL
            
        self._history.append(close)
        
        # Trim history
        slow = self.params['slow_period']
        # Keep enough for rolling calc
        if len(self._history) > slow + 20: 
            self._history.pop(0)
            
        if len(self._history) < slow:
            return SignalType.NEUTRAL
            
        # Calculate SMAs
        series = pd.Series(self._history)
        fast_ma = series.rolling(window=self.params['fast_period']).mean().iloc[-1]
        slow_ma = series.rolling(window=self.params['slow_period']).mean().iloc[-1]
        
        # Determine Signal (Basic Crossover Logic)
        # Note: Ideally we check previous candle to confirm the "Cross" happened just now.
        # But for this simple implementation, we just check current state.
        if fast_ma > slow_ma:
            return SignalType.LONG
        elif fast_ma < slow_ma:
            return SignalType.SHORT
        
        return SignalType.NEUTRAL
