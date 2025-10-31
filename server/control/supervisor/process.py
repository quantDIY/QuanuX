from __future__ import annotations
import os, sys, shlex, subprocess
from dataclasses import asdict
from typing import List
from pathlib import Path

from server.control.models import BridgeSpec, BridgeStatus
from server.control import registry as reg


# Map (name, runtime) -> command string
def _command_for(spec: BridgeSpec) -> List[str]:
    name = spec.name
    runtime = spec.runtime
    if name == "signalr" and runtime == "flask":
        cmd = "python3 server/bridges/signalr/flask/app.py"
    elif name == "signalr" and runtime == "node":
        cmd = "node server/bridges/signalr/node/index.mjs"
    else:
        raise ValueError(f"Unsupported bridge '{name}' runtime '{runtime}'")
    return shlex.split(cmd)


def start(spec: BridgeSpec, node: str = "local") -> BridgeStatus:
    # If already up, return status
    pid = reg.read_pid(spec.name)
    if pid and reg.process_alive(pid):
        return BridgeStatus(name=spec.name, runtime=spec.runtime, pid=pid, port=spec.port, alive=True, healthy=True, node=node)

    env = os.environ.copy()
    env["QUANUX_BRIDGE_RUNTIME"] = spec.runtime
    env["QUANUX_BRIDGE_PORT"] = str(spec.port)
    if spec.env:
        env.update(spec.env)

    cmd = _command_for(spec)
    proc = subprocess.Popen(cmd, env=env)  # noqa: S603,S607
    reg.write_pid(spec.name, proc.pid)
    reg.write_meta(spec.name, {"runtime": spec.runtime, "port": spec.port, "env": spec.env or {}})

    return BridgeStatus(name=spec.name, runtime=spec.runtime, pid=proc.pid, port=spec.port, alive=True, healthy=True, node=node)


def stop(name: str, node: str = "local") -> BridgeStatus:
    meta = reg.read_meta(name)
    pid = reg.read_pid(name)
    runtime = meta.get("runtime", "flask")
    port = int(meta.get("port", 0) or 0)

    if pid and reg.process_alive(pid):
        reg.terminate(pid)
    reg.clear_pid(name)

    # Re-check alive state
    alive = pid is not None and reg.process_alive(pid)
    return BridgeStatus(name=name, runtime=runtime, pid=pid, port=port, alive=alive, healthy=not alive, node=node)


def status(name: str, node: str = "local") -> BridgeStatus:
    meta = reg.read_meta(name)
    pid = reg.read_pid(name)
    runtime = meta.get("runtime", "flask")
    port = int(meta.get("port", 0) or 0)
    alive = bool(pid and reg.process_alive(pid))
    return BridgeStatus(name=name, runtime=runtime, pid=pid, port=port, alive=alive, healthy=alive, node=node)


def list_bridges(node: str = "local") -> list[dict]:
    root = Path("server/control/state")
    if not root.exists():
        return []
    out = []
    for d in root.iterdir():
        if d.is_dir():
            st = status(d.name, node=node)
            out.append(st.to_dict())
    return out


# Legacy CLI entry (optional)
def main(argv: list[str] | None = None) -> int:
    import argparse, json
    p = argparse.ArgumentParser(prog="bridge-supervisor")
    sub = p.add_subparsers(dest="cmd", required=True)

    up = sub.add_parser("up")
    up.add_argument("--name", default="signalr")
    up.add_argument("--runtime", choices=["flask", "node"], default="flask")
    up.add_argument("--port", type=int, default=8077)

    down = sub.add_parser("down")
    down.add_argument("--name", default="signalr")

    st = sub.add_parser("status")
    st.add_argument("--name", default="signalr")

    sub.add_parser("list")

    args = p.parse_args(argv)

    if args.cmd == "up":
        res = start(BridgeSpec(name=args.name, runtime=args.runtime, port=args.port))
        print(json.dumps(res.to_dict(), indent=2))
        return 0
    if args.cmd == "down":
        res = stop(args.name)
        print(json.dumps(res.to_dict(), indent=2))
        return 0
    if args.cmd == "status":
        res = status(args.name)
        print(json.dumps(res.to_dict(), indent=2))
        return 0
    if args.cmd == "list":
        print(json.dumps(list_bridges(), indent=2))
        return 0
    return 2


if __name__ == "__main__":
    raise SystemExit(main())
