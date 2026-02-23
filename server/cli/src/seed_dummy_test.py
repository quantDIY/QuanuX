import sys
from pathlib import Path

repo_root = Path(__file__).resolve().parent.parent.parent.parent
engine_path = repo_root / "QuanuX-Backtesting-Engine" / "python"
sys.path.insert(0, str(engine_path))

import quanux_backtest.cython.quanux_crucible as qc
import random
import os

strategy = "dummy"
version = "1.0.0"

db_dir = repo_root / "server" / "backtests" / f"{strategy}_v{version}"
db_dir.mkdir(parents=True, exist_ok=True)
db_path = db_dir / "crucible.duckdb"

# Cleanup old test
if db_path.exists():
    os.remove(db_path)

print(f"Loading DuckDB at {db_path} via Cython...")
feeder = qc.PyDuckDBFeeder(str(db_path))
feeder.init_appenders()

trades = []
pnl = 0.0
for i in range(500):
    t = qc.PyCrucibleTrade()
    
    # 60% win rate
    is_win = random.random() < 0.60
    
    if is_win:
        t.profit = random.uniform(50.0, 150.0)
    else:
        # Keep losses slightly larger to test standard R:R
        t.profit = random.uniform(-100.0, -200.0)
        
    t.entry_time_ns = i * 1000000
    t.exit_time_ns = (i * 1000000) + 50000
    t.isLong = random.choice([True, False])
    t.size = 1
    t.entryPrice = 4000.0 + random.uniform(-10.0, 10.0)
    t.exitPrice = t.entryPrice + (t.profit if t.isLong else -t.profit)
    t.latency_slippage_bps = random.uniform(0.1, 1.5)
    
    # Excursions
    t.mae = random.uniform(0, 50.0) if is_win else random.uniform(100.0, 200.0)
    t.mfe = random.uniform(50.0, 250.0) if is_win else random.uniform(0, 25.0)
    t.queue_position_at_entry = random.randint(1, 150)
    
    trades.append(t)

print("Appending 500 fake highly volatile trades into DuckDB C++ Core...")
feeder.append_trades(strategy, trades)

print("Done generating dummy data.")
