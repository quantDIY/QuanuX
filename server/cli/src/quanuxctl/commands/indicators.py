#!/usr/bin/env python3
from __future__ import annotations
import argparse, json, sys
import pandas as pd
from typing import List

# Import via the repo "server" package; ensure repo root is on PYTHONPATH when running
from server.indicators.ta.loader import load_backend
from server.indicators.ta.indicators import sma, rsi, macd, ema

def _parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(prog="quanuxctl indicators", description="Indicators utilities")
    sub = p.add_subparsers(dest="cmd", required=True)

    sub.add_parser("probe", help="Show which backend will be used")

    demo = sub.add_parser("demo-sma", help="Compute SMA on random walk data")
    demo.add_argument("--length", type=int, default=20)
    demo.add_argument("--rows", type=int, default=200)
    return p

def _demo_series(rows: int) -> pd.Series:
    import numpy as np
    rng = np.random.default_rng(7)
    steps = rng.normal(loc=0.0, scale=1.0, size=rows)
    close = pd.Series(100 + steps.cumsum(), name="close")
    return close

def cmd_indicators(argv: List[str]) -> int:
    args = _parser().parse_args(argv)

    if args.cmd == "probe":
        info, _ = load_backend()
        print(json.dumps({
            "backend": info.name, "version": info.version,
            "ready": info.ready, "note": info.note
        }, indent=2))
        return 0

    if args.cmd == "demo-sma":
        s = _demo_series(args.rows)
        out = sma(s, length=args.length)
        print(pd.DataFrame({"close": s, f"SMA_{args.length}": out}).tail(10).to_string())
        return 0

    print("unknown subcommand")
    return 2

if __name__ == "__main__":
    sys.exit(cmd_indicators(sys.argv[1:]))
