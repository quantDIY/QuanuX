# distutils: language = c++
# cython: language_level = 3

import asyncio

cdef class Adapter:
    """
    Abstract Base Class for Data Sources and Execution Venues.
    Strategies interact with THIS interface, ensuring portability.
    """
    
    def __init__(self, name="base", config=None):
        self.name = name
        self.config = config or {}

    async def connect(self):
        """Establish connection to data/execution venue."""
        raise NotImplementedError("Subclasses must implement connect()")

    async def subscribe(self, list symbols):
        """Subscribe to market data."""
        raise NotImplementedError("Subclasses must implement subscribe()")

    async def place_order(self, dict order_data):
        """Place an order."""
        raise NotImplementedError("Subclasses must implement place_order()")
        
    # Events callback registration
    # Strategy will attach handlers here.
    # We use python method references for simplicity in v1.
    
    async def on_tick(self, dict tick):
        """Called by subclass implementation when data arrives."""
        pass 
