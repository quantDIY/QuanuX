#!/usr/bin/env python3
from __future__ import annotations
import os, subprocess, sys, shlex, argparse
from pathlib import Path
from typing import Optional
# from server.control.supervisor.process import main

ROOT = Path(__file__).resolve().parent  # extensions/python/signalr_bridge/src
FLASK_APP = ROOT / "flask" / "app.py"
NODE_APP  = ROOT / "node" / "index.mjs"

def run_cmd(cmd: str, env: Optional[dict] = None) -> int:
    proc = subprocess.Popen(shlex.split(cmd), env=env or os.environ.copy())
    try:
        return proc.wait()
    except KeyboardInterrupt:
        proc.terminate()
        try:
            proc.wait(timeout=3)
        except Exception:
            proc.kill()
        return 130

def cmd_up(runtime: str, port: int) -> int:
    os.environ["QUANUX_BRIDGE_PORT"] = str(port)
    if runtime == "flask":
        cmd = f"python3 {FLASK_APP}"
    elif runtime == "node":
        cmd = f"node {NODE_APP}"
    else:
        print(f"Unknown runtime: {runtime}", file=sys.stderr)
        return 2
    print(f"[supervisor] starting SignalR bridge ({runtime}) on :{port}")
    return run_cmd(cmd)

def cmd_down() -> int:
    # Scaffold: no PID tracking yet. Integrate PID files/systemd later.
    print("[supervisor] down requested (scaffold). No tracked process.")
    return 0

def main(argv=None) -> int:
    p = argparse.ArgumentParser(prog="signalr-supervisor")
    p.add_argument("--down", action="store_true", help="stop bridge (scaffold)")

    args, rest = p.parse_known_args(argv)

    if args.down:
        return cmd_down()

    runtime = os.getenv("QUANUX_BRIDGE_RUNTIME", "flask")
    port = int(os.getenv("QUANUX_BRIDGE_PORT", "8077"))
    return cmd_up(runtime, port)


if __name__ == "__main__":
    raise SystemExit(main())

#if __name__ == "__main__":
 #   raise SystemExit(main(sys.argv[1:]))
