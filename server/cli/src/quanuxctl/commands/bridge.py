#!/usr/bin/env python3
from __future__ import annotations
"""
quanuxctl bridge — manage the SignalR bridge (scaffold)
Usage:
  quanuxctl bridge up [--runtime flask|node] [--port 8077]
  quanuxctl bridge down
"""

import argparse, os, sys, subprocess, shlex
from pathlib import Path

# Correct path: supervisor lives at repo/server/bridges/signalr/supervisor.py
# We are in repo/server/cli/src/quanuxctl/commands/, so we go up 5 levels to the repo root.
REPO_ROOT = Path(__file__).resolve().parents[5]
SUPERVISOR = REPO_ROOT / "server" / "bridges" / "signalr" / "supervisor.py"

def _run(cmd: str, env=None) -> int:
    return subprocess.call(shlex.split(cmd), env=env or os.environ.copy())

def cmd_bridge(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(prog="quanuxctl bridge", description="Manage SignalR bridge (scaffold)")
    sub = parser.add_subparsers(dest="cmd", required=True)

    up_p = sub.add_parser("up", help="start the bridge")
    up_p.add_argument("--runtime", choices=["flask", "node"], default="flask")
    up_p.add_argument("--port", type=int, default=8077)

    sub.add_parser("down", help="stop the bridge (scaffold)")

    args = parser.parse_args(argv)

    if args.cmd == "up":
        env = os.environ.copy()
        env["QUANUX_BRIDGE_PORT"] = str(args.port)
        env["QUANUX_BRIDGE_RUNTIME"] = args.runtime
        return _run(f"python3 {SUPERVISOR}", env)
    elif args.cmd == "down":
        return _run(f"python3 {SUPERVISOR} --down")
    else:
        parser.print_help()
        return 2

if __name__ == "__main__":
    sys.exit(cmd_bridge(sys.argv[1:]))
