# distutils: language = c++
# cython: language_level = 3

import asyncio
import importlib.util
import sys
import os

from adapter cimport Adapter

cdef class Core:
    """
    Main Runtime for the Cython Execution Node.
    - Loads Strategy (.so)
    - Initializes Adapter (Direct or Relay)
    - Runs Event Loop
    """
    cdef public object strategy
    cdef public Adapter adapter
    cdef public object loop

    def __init__(self):
        self.strategy = None
        self.adapter = None
        self.loop = asyncio.new_event_loop()
        asyncio.set_event_loop(self.loop)

    def load_strategy(self, str path):
        """Dynamically load a compiled strategy module."""
        name = os.path.basename(path).split('.')[0]
        spec = importlib.util.spec_from_file_location(name, path)
        mod = importlib.util.module_from_spec(spec)
        sys.modules[name] = mod
        spec.loader.exec_module(mod)
        
        # Assume strategy has a class 'Strategy'
        print(f"Loaded strategy module: {name}")
        self.strategy = mod.Strategy()
        
    def set_adapter(self, Adapter adapter):
        self.adapter = adapter
        
    async def run(self):
        """Main lifecycle."""
        print("Starting Node Core...")
        
        # 1. Connect Adapter
        await self.adapter.connect()
        
        # 2. Initialize Strategy
        if hasattr(self.strategy, "on_start"):
            await self.strategy.on_start(self.adapter)
            
        print("Node Running. Press Ctrl+C to stop.")
        # Keep alive
        while True:
            await asyncio.sleep(1)

    def start(self):
        try:
            self.loop.run_until_complete(self.run())
        except KeyboardInterrupt:
            print("Stopping...")
