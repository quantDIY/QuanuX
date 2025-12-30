
from signals import SignalGenerator
from entry import EntryModule
from risk import RiskManager
from sizing import PositionSizingModule

class MyStrategy:
    def __init__(self):
        self.signal = SignalGenerator()
        self.entry = EntryModule()
        self.risk = RiskManager()
        self.sizing = PositionSizingModule()
        
        self.ticks = []
        self.position = 0 # 0=Flat, 1=Long, -1=Short
        self.done = False

    def on_tick(self, price: float):
        self.ticks.append(price)
        if len(self.ticks) > 10:
            self.ticks.pop(0) # Keep window small
            
        # Signal Logic: 3 consecutive upticks
        if len(self.ticks) >= 4:
            # t-3, t-2, t-1, t-0
            # if t0 > t1 > t2 > t3
            if (self.ticks[-1] > self.ticks[-2] and 
                self.ticks[-2] > self.ticks[-3] and 
                self.ticks[-3] > self.ticks[-4]):
                
                if self.position == 0:
                    return "BUY"
                    
        return None

    def on_fill(self, action, price, qty):
        if action == "BUY":
            self.position += qty
            print(f"Strategy: Filled LONG @ {price}")
            self.done = True # Stop after one trade for this test
        elif action == "SELL":
            self.position -= qty
            
