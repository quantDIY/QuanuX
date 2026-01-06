#!/usr/bin/env python3
from __future__ import annotations
import os, subprocess, sys, shlex, argparse
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parents[3]  # repo/
APP_PATH = ROOT / "server" / "bridges" / "async_rithmic_py" / "app.py"

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

def cmd_up(port: int) -> int:
    os.environ["QUANUX_BRIDGE_PORT"] = str(port)
    cmd = f"python3 {APP_PATH}"
    print(f"[supervisor] starting Rithmic bridge on :{port}")
    return run_cmd(cmd)

def cmd_down() -> int:
    # Scaffold: no PID tracking yet. Integrate PID files/systemd later.
    print("[supervisor] down requested (scaffold). No tracked process.")
    return 0

def main(argv=None) -> int:
    p = argparse.ArgumentParser(prog="rithmic-supervisor")
    p.add_argument("--down", action="store_true", help="stop bridge (scaffold)")

    args, rest = p.parse_known_args(argv)

    if args.down:
        return cmd_down()

    port = int(os.getenv("QUANUX_BRIDGE_PORT", "8078"))
    return cmd_up(port)

if __name__ == "__main__":
    raise SystemExit(main())
