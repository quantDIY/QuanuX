
"""
QuanuX Backtest Metrics Example
"""
from typing import List, Dict
import pandas as pd
import numpy as np

# Import the wrapper
try:
    from .analysis import BacktestAnalyzer
except ImportError:
    # If running as script
    from quanux_backtest.analysis import BacktestAnalyzer

def analyze_example():
    print("Generating sample data...")
    
    # Generate a random walk equity curve
    np.random.seed(42)
    returns = np.random.normal(0.001, 0.02, 1000) # Mean 0.1%, Vol 2%
    equity = 10000.0 * np.cumprod(1 + returns)
    
    # Create Series
    equity_series = pd.Series(equity)
    
    print("Running C++ Analysis via Python Wrapper...")
    analyzer = BacktestAnalyzer(start_equity=10000.0)
    metrics = analyzer.process_equity(equity_series)
    
    print("\n--- Results ---")
    for k, v in metrics.items():
        print(f"{k:<20}: {v:.4f}")

if __name__ == "__main__":
    analyze_example()
