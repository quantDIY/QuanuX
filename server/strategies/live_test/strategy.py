from datetime import datetime
from typing import Optional, Dict, Any

class TestStrategy:
    def __init__(self):
        self.tick_count = 0
        self.entry_price: Optional[float] = None
        self.position: int = 0  # 0: Flat, 1: Long, -1: Short
        self.stop_price: Optional[float] = None
        self.target_price: Optional[float] = None
        self.trade_placed = False
        self.done = False

    def on_tick(self, price: float):
        if self.done:
            return None

        self.tick_count += 1
        print(f"[Strategy] Tick {self.tick_count}: {price}")

        # Logic: Wait for 3 ticks then Buy
        if not self.trade_placed and self.tick_count >= 3:
            self.trade_placed = True
            return "BUY"

        # Logic: Check Exits
        if self.position != 0:
            if self.position == 1: # Long
                if price <= self.stop_price:
                    print(f"[Strategy] Stop Loss Hit at {price}")
                    self.done = True
                    return "SELL" # Close
                elif price >= self.target_price:
                    print(f"[Strategy] Take Profit Hit at {price}")
                    self.done = True
                    return "SELL" # Close
            
        return None

    def on_fill(self, action: str, price: float, quantity: int):
        print(f"[Strategy] Filled {action} @ {price}")
        if action == "BUY":
            self.position = 1
            self.entry_price = price
            # Set bracket
            self.stop_price = price - 10.0 # 10 pts stop
            self.target_price = price + 20.0 # 20 pts target
            print(f"[Strategy] Bracket Set: Stop={self.stop_price}, Target={self.target_price}")
        elif action == "SELL":
            self.position = 0
            self.done = True
            print("[Strategy] Position Closed. Strategy Done.")

