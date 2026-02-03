import asyncio
import sys
import os

# Add parent to path to find .so modules
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

try:
    from core import Core
    from adapter import Adapter
except ImportError as e:
    print(f"Import Error: {e}")
    sys.exit(1)

class MockAdapter(Adapter):
    async def connect(self):
        print("MockAdapter Connected")
    
    async def place_order(self, order):
        print(f"MockAdapter Placed Order: {order}")

async def main():
    core = Core()
    
    # 1. Set Adapter
    adapter = MockAdapter(name="mock_v1")
    core.set_adapter(adapter)
    
    # 2. Load Strategy
    strat_path = os.path.join(os.path.dirname(__file__), "dummy_strategy.py")
    core.load_strategy(strat_path)
    
    # 3. Run (Short lifecycle for test)
    print("Running Core...")
    # launching run in background task to kill it after 2 seconds
    task = asyncio.create_task(core.run())
    
    await asyncio.sleep(2)
    print("Test Complete. Stopping.")
    # Cancel infinite loop
    task.cancel()
    try:
        await task
    except asyncio.CancelledError:
        pass

if __name__ == "__main__":
    asyncio.run(main())
