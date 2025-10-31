#!/usr/bin/env python3
"""
quanuxctl bridge — manage bridges via NATS (req/reply)

Usage:
  quanuxctl bridge up [--runtime flask|node] [--port 8077] [--name <bridge>] [--env K=V ...]
  quanuxctl bridge down [--name <bridge>]
  quanuxctl bridge status [--name <bridge>]
  quanuxctl bridge list
"""

from __future__ import annotations

import argparse
import asyncio
import json
import os
import shlex
import subprocess
import sys
from pathlib import Path
from typing import Dict

import nats  # nats-py


# NOTE: kept for parity with earlier local-supervisor flow (not used when talking to NATS)
REPO_ROOT = Path(__file__).resolve().parents[5]
SUPERVISOR = REPO_ROOT / "server" / "bridges" / "signalr" / "supervisor.py"


def _run(cmd: str, env=None) -> int:
    return subprocess.call(shlex.split(cmd), env=env or os.environ.copy())


def _parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="quanuxctl bridge",
        description="Manage bridges via NATS (req/reply)"
    )
    p.add_argument(
        "--node",
        default=os.getenv("QUANUX_NODE_ID", "local"),
        help="target node id (default: local)"
    )
    sub = p.add_subparsers(dest="cmd", required=True)

    def add_common(sp):
        sp.add_argument("--name", default="signalr", help="bridge name (default: signalr)")

    up = sub.add_parser("up", help="start bridge")
    add_common(up)
    up.add_argument("--runtime", choices=["flask", "node"], default="flask")
    up.add_argument("--port", type=int, default=8077)
    up.add_argument("--env", action="append", default=[], help="k=v (repeatable)")

    down = sub.add_parser("down", help="stop bridge")
    add_common(down)

    st = sub.add_parser("status", help="bridge status")
    add_common(st)

    sub.add_parser("list", help="list bridges on node")

    return p


def _env_kv_to_dict(items: list[str]) -> Dict[str, str]:
    out: Dict[str, str] = {}
    for item in items:
        if "=" in item:
            k, v = item.split("=", 1)
            out[k] = v
    return out


async def _req(subject: str, payload: dict, nats_url: str, timeout: float = 1.5) -> dict:
    nc = await nats.connect(servers=[nats_url])
    try:
        msg = await nc.request(subject, json.dumps(payload).encode(), timeout=timeout)
        return json.loads(msg.data.decode() or "{}")
    finally:
        await nc.drain()


def cmd_bridge(argv: list[str]) -> int:
    args = _parser().parse_args(argv)
    nats_url = os.getenv("NATS_URL", "nats://127.0.0.1:4222")

    if args.cmd == "up":
        env = _env_kv_to_dict(args.env)
        payload = {"name": args.name, "runtime": args.runtime, "port": args.port, "env": env}
        subject = f"control.{args.node}.bridge.up"
    elif args.cmd == "down":
        payload = {"name": args.name}
        subject = f"control.{args.node}.bridge.down"
    elif args.cmd == "status":
        payload = {"name": args.name}
        subject = f"control.{args.node}.bridge.status"
    elif args.cmd == "list":
        payload = {}
        subject = f"control.{args.node}.bridge.list"
    else:
        print("Unknown subcommand")
        return 2

    try:
        res = asyncio.run(_req(subject, payload, nats_url))
    except Exception as e:
        print(f"Error contacting control plane at {nats_url}: {e}")
        return 1

    print(json.dumps(res, indent=2))
    return 0


if __name__ == "__main__":
    sys.exit(cmd_bridge(sys.argv[1:]))
