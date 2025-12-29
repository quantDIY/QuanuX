from abc import ABC, abstractmethod
from typing import Dict, Any, Optional, List
from enum import Enum
from pydantic import BaseModel, Field

class SignalType(str, Enum):
    LONG = "LONG"
    SHORT = "SHORT"
    NEUTRAL = "NEUTRAL"

class BaseParameters(BaseModel):
    """Base class for all component parameters."""
    pass

class StrategyComponent(ABC):
    """
    Base class for all modular strategy components (Signal, Entry, Exit, Risk).
    """
    def __init__(self, name: str, params: Optional[Dict[str, Any]] = None):
        self.name = name
        self.params = params or {}
        self._param_model = self.define_parameters()
        if self.params:
            self.validate_parameters()

    @abstractmethod
    def define_parameters(self) -> type[BaseParameters]:
        """
        Returns a Pydantic model class defining the expected parameters.
        """
        pass

    def validate_parameters(self):
        """
        Validates the current self.params against the defined Pydantic model.
        """
        if self._param_model:
            # parsing will raise ValidationError if invalid
            validated = self._param_model(**self.params)
            # update params with validated (and potentially defaulted) values
            self.params = validated.model_dump()

    def update_parameters(self, new_params: Dict[str, Any]):
        """
        Dynamically updates parameters at runtime.
        """
        self.params.update(new_params)
        self.validate_parameters()

class SignalModule(StrategyComponent):
    """
    Responsible for analyzing market data and generating a signal (Long/Short/Neutral).
    """
    @abstractmethod
    def on_bar(self, bar_data: Any) -> SignalType:
        """
        Called on every new bar. Returns the signal for the current state.
        """
        pass

class PositionSizingModule(StrategyComponent):
    """
    Responsible for calculating the size of the position (e.g. quantity of contracts/shares).
    """
    @abstractmethod
    def calculate_size(self, signal: SignalType, price: float, account_equity: float) -> float:
        """
        Returns the quantity to trade.
        """
        pass

class EntryModule(StrategyComponent):
    """
    Responsible for generating entry orders based on a signal.
    """
    @abstractmethod
    def generate_entry(self, signal: SignalType, bar_data: Any) -> Optional[Dict[str, Any]]:
        """
        Returns an order dictionary if an entry should be made, else None.
        """
        pass

class RiskModule(StrategyComponent):
    """
    Responsible for calculating Stop Loss and Take Profit levels or Orders.
    """
    @abstractmethod
    def calculate_risk(self, entry_price: float, signal: SignalType, bar_data: Any) -> Dict[str, float]:
        """
        Returns a dict with 'stop_loss', 'take_profit', 'trailing_stop' etc.
        """
        pass

class ExitModule(StrategyComponent):
    """
    Responsible for checking if an open position should be closed (independent of fixed stops/limits).
    e.g. Time-based exit, or Indicator-based exit.
    """
    @abstractmethod
    def should_exit(self, position: Any, bar_data: Any) -> bool:
        """
        Returns True if the position should be closed immediately.
        """
        pass

class CompositeStrategy:
    """
    The container that orchestrates the execution of modular components.
    """
    def __init__(
        self, 
        name: str, 
        signal_module: SignalModule, 
        entry_module: EntryModule, 
        risk_module: RiskModule, 
        exit_module: Optional[ExitModule] = None,
        position_sizing_module: Optional[PositionSizingModule] = None
    ):
        self.name = name
        self.signal_module = signal_module
        self.entry_module = entry_module
        self.risk_module = risk_module
        self.exit_module = exit_module
        self.position_sizing_module = position_sizing_module
        
        self.position = None # Placeholder for current position state

    def on_bar(self, bar_data: Any):
        """
        Main loop execution.
        """
        # 1. Check Exit if we have a position
        if self.position and self.exit_module:
            if self.exit_module.should_exit(self.position, bar_data):
                print(f"[{self.name}] Exit Signal triggered by {self.exit_module.name}")
                # emit close order logic here
                self.position = None
                return

        # 2. Get Signal
        signal = self.signal_module.on_bar(bar_data)
        
        # 3. Check Entry if flat and signal exists
        if not self.position and signal != SignalType.NEUTRAL:
            entry_order = self.entry_module.generate_entry(signal, bar_data)
            if entry_order:
                # 4. Calculate Size
                price = bar_data.get('close', 0)
                size = 1.0
                if self.position_sizing_module:
                    # Mock equity = 100k
                    size = self.position_sizing_module.calculate_size(signal, price, 100000.0)
                
                # 5. Calculate Risk Params
                risk_levels = self.risk_module.calculate_risk(price, signal, bar_data)
                
                print(f"[{self.name}] Entry Triggered: {signal} at {price}. Size: {size}. Risk: {risk_levels}")
                # In a real engine, this would send the order + OCO brackets
                self.position = {"side": signal, "entry_price": price, "size": size, **risk_levels}
