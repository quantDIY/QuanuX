import pandas as pd
import sys
import os
from typing import Optional, Dict

# Try to import the C++ extension
# We assume it's available in the python path or adjacent
try:
    import quanux_metrics
except ImportError:
    # Fallback: Look in relative build directory (for development)
    # This logic helps when running directly from source
    current_dir = os.path.dirname(os.path.abspath(__file__))
    # Adjust this path based on where build output goes
    build_path = os.path.abspath(os.path.join(current_dir, "../../cpp/build"))
    if os.path.exists(build_path):
        sys.path.append(build_path)
        try:
            import quanux_metrics
        except ImportError:
            raise ImportError(
                "Could not import 'quanux_metrics'. Ensure the C++ extension is built."
            )
    else:
        raise

class BacktestAnalyzer:
    """
    High-level Python wrapper for QuanuX C++ Metrics Engine.
    Accepts Pandas DataFrames and returns structured metrics.
    """

    def __init__(self, start_equity: float = 10000.0, risk_free_rate: float = 0.0):
        self.cpp_analyzer = quanux_metrics.PerformanceAnalyzer(start_equity, risk_free_rate)
        self.start_equity = start_equity

    def process_trades(self, trades_df: pd.DataFrame) -> Dict:
        """
        Process a DataFrame of trades and return metrics.
        Expected columns: 'profit', 'entry_price', 'exit_price', 'risk' (optional), 'duration' (optional)
        """
        if trades_df.empty:
            return {}

        for _, row in trades_df.iterrows():
            t = quanux_metrics.Trade()
            t.profit = float(row.get('profit', 0.0))
            t.entryPrice = float(row.get('entry_price', 0.0))
            t.exitPrice = float(row.get('exit_price', 0.0))
            
            # Optional fields
            t.risk = float(row.get('risk', 0.0))
            t.durationBars = int(row.get('duration', 0))
            t.isLong = True if row.get('side', 'LONG') == 'LONG' else False
            
            self.cpp_analyzer.addTrade(t)

        return self.calculate()

    def process_equity(self, equity_series: pd.Series) -> Dict:
        """
        Process a time-series of equity values.
        """
        if equity_series.empty:
            return {}
            
        # We assume the series is in chronological order
        for val in equity_series:
            self.cpp_analyzer.updateEquity(float(val))
            
        return self.calculate()

    def calculate(self, periods_per_year: int = 252) -> Dict:
        """
        Trigger C++ calculation and return dictionary.
        """
        m = self.cpp_analyzer.calculateMetrics(periods_per_year)
        
        return {
            "net_profit": m.netProfit,
            "gross_profit": m.grossProfit,
            "gross_loss": m.grossLoss,
            "profit_factor": m.profitFactor,
            "win_rate": m.winRate,
            "sharpe_ratio": m.sharpeRatio,
            "sortino_ratio": m.sortinoRatio,
            "calmar_ratio": m.calmarRatio,
            "omega_ratio": m.omegaRatio,
            "ulcer_index": m.ulcerIndex,
            "max_drawdown_pct": m.maxDrawdownPct,
            "sqn": m.sqn,
            "cagr": m.cagr
        }

def quick_analyze(equity_curve: pd.Series) -> Dict:
    """Helper for quick analysis of an equity curve."""
    analyzer = BacktestAnalyzer(start_equity=equity_curve.iloc[0])
    return analyzer.process_equity(equity_curve)
