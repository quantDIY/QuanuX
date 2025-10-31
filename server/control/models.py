from __future__ import annotations
from dataclasses import dataclass, asdict
from typing import Dict, Optional


@dataclass
class BridgeSpec:
    name: str = "signalr"
    runtime: str = "flask"          # "flask" | "node"
    port: int = 8077
    env: Dict[str, str] = None

    def to_dict(self) -> dict:
        d = asdict(self)
        d["env"] = d.get("env") or {}
        return d


@dataclass
class BridgeStatus:
    name: str
    runtime: str
    pid: Optional[int]
    port: int
    alive: bool
    healthy: bool
    node: str = "local"

    def to_dict(self) -> dict:
        return asdict(self)
