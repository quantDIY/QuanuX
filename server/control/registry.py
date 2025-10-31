from __future__ import annotations
import json, os, signal
from pathlib import Path
from typing import Optional


_STATE_ROOT = Path("server/control/state").resolve()


def _bridge_dir(name: str) -> Path:
    d = _STATE_ROOT / name
    d.mkdir(parents=True, exist_ok=True)
    return d


def write_pid(name: str, pid: int) -> None:
    p = _bridge_dir(name) / "pid"
    p.write_text(str(pid), encoding="utf-8")


def read_pid(name: str) -> Optional[int]:
    p = _bridge_dir(name) / "pid"
    if not p.exists():
        return None
    try:
        return int(p.read_text(encoding="utf-8").strip())
    except Exception:
        return None


def clear_pid(name: str) -> None:
    p = _bridge_dir(name) / "pid"
    if p.exists():
        p.unlink()


def write_meta(name: str, data: dict) -> None:
    (_bridge_dir(name) / "meta.json").write_text(json.dumps(data, indent=2), encoding="utf-8")


def read_meta(name: str) -> dict:
    p = _bridge_dir(name) / "meta.json"
    if p.exists():
        try:
            return json.loads(p.read_text(encoding="utf-8"))
        except Exception:
            return {}
    return {}


def process_alive(pid: int) -> bool:
    try:
        os.kill(pid, 0)  # does not kill; raises if not alive
        return True
    except Exception:
        return False


def terminate(pid: int, timeout_s: float = 2.0) -> None:
    try:
        os.kill(pid, signal.SIGTERM)
    except ProcessLookupError:
        return
