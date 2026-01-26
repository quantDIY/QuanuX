import sys
import os

# Add build directory to path to find the extension
# Assumes build/ is inside cpp/ relative to this script
script_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = os.path.join(script_dir, "cpp", "build")
sys.path.append(build_dir)

print(f"Looking for quanux_metrics in {build_dir}")

try:
    import quanux_metrics
    print("SUCCESS: Imported quanux_metrics")
except ImportError as e:
    print(f"ERROR: Could not import quanux_metrics: {e}")
    # Try to list files in build_dir to debug
    if os.path.exists(build_dir):
        print(f"Contents of {build_dir}:")
        for f in os.listdir(build_dir):
             if "quanux_metrics" in f:
                 print(f"  FOUND: {f}")
    sys.exit(1)

def test_metrics():
    print("\n--- Testing PerformanceAnalyzer ---")
    analyzer = quanux_metrics.PerformanceAnalyzer(startingEquity=10000.0)
    
    # Simulate some trades: 3 wins, 2 losses
    equity = 10000.0
    analyzer.updateEquity(equity)
    
    # Simple sequence of trades + equity updates
    trades_data = [
        (100.0, 110.0, 100.0),   # +100
        (110.0, 105.0, -50.0),   # -50
        (105.0, 120.0, 150.0),   # +150
        (120.0, 130.0, 100.0),   # +100
        (130.0, 125.0, -50.0),   # -50
    ]
    # Final Equity: 10000 + 100 - 50 + 150 + 100 - 50 = 10250.0
    
    for entry, exit_p, profit in trades_data:
        t = quanux_metrics.Trade()
        t.entryPrice = entry
        t.exitPrice = exit_p
        t.profit = profit
        t.profitPct = (exit_p - entry) / entry
        t.risk = 50.0 # Assume $50 risk per trade
        t.isLong = True
        t.durationBars = 10
        
        analyzer.addTrade(t)
        
        equity += profit
        analyzer.updateEquity(equity)
        
    metrics = analyzer.calculateMetrics()
    
    print(f"Object: {metrics}")
    print(f"Net Profit: {metrics.netProfit}")
    print(f"Win Rate: {metrics.winRate}")
    print(f"Sharpe Ratio: {metrics.sharpeRatio:.4f}")
    print(f"Sortino Ratio: {metrics.sortinoRatio:.4f}")
    print(f"SQN: {metrics.sqn:.4f}")
    print(f"Ulcer Index: {metrics.ulcerIndex:.4f}")
    
    assert metrics.netProfit == 250.0
    assert metrics.winRate == 0.6  # 3 wins, 2 losses -> 3/5 = 0.6
    
    print("\nTEST PASSED")

if __name__ == "__main__":
    test_metrics()
