#!/usr/bin/env python3
"""
QuanuX Control (quanuxctl) — scaffold dispatcher.
"""
from __future__ import annotations
import os, sys, traceback

HELP = """\
QuanuX Control (quanuxctl) — scaffold v0.0.1
indicators  Indicators utilities (probe, demo-sma)

Usage:
  quanuxctl <command>

Commands:
  start        Print how to start the dev server (scaffold)
  build        Placeholder for Suite build (scaffold)
  diagnose     Env diagnostics (scaffold)
  ai-train     AI docs index hook (scaffold)
  pack-docs    Bundle docs (no-op scaffold)
  docs         List docs entry points
  backlog      Print backlog summary
  bridge       Manage SignalR bridge (flask|node)
"""

def _try_import(module: str, func: str):
    try:
        mod = __import__(module, fromlist=[func])
        return getattr(mod, func)
    except Exception:
        if os.getenv("QUANUX_DEBUG") == "1":
            print(f"[DEBUG] Failed importing {module}:{func}", file=sys.stderr)
            traceback.print_exc()
        return None

def _missing(name: str) -> int:
    print(f"{name} scaffold not available yet.")
    return 1

def main(argv=None) -> int:
    cmd_start     = _try_import("quanuxctl.commands.start", "cmd_start")
    cmd_build     = _try_import("quanuxctl.commands.build", "cmd_build")
    cmd_diag      = _try_import("quanuxctl.commands.diagnose", "cmd_diagnose")
    cmd_ai_train  = _try_import("quanuxctl.commands.ai_train", "cmd_ai_train")
    cmd_pack_docs = _try_import("quanuxctl.commands.pack_docs", "cmd_pack_docs")
    cmd_docs      = _try_import("quanuxctl.commands.docs", "cmd_docs")
    cmd_backlog   = _try_import("quanuxctl.commands.docs", "cmd_backlog")
    cmd_bridge    = _try_import("quanuxctl.commands.bridge", "cmd_bridge")
    cmd_ind       = _try_import("quanuxctl.commands.indicators", "cmd_indicators")  # <— NEW

    args = list(sys.argv[1:] if argv is None else argv)
    if not args or args[0] in {"-h", "--help", "help"}:
        print(HELP)
        return 0

    cmd, rest = args[0], args[1:]

    if cmd == "start":
        return cmd_start(rest) if cmd_start else _missing("start")
    if cmd == "build":
        return cmd_build(rest) if cmd_build else _missing("build")
    if cmd == "diagnose":
        return cmd_diag(rest) if cmd_diag else _missing("diagnose")
    if cmd in {"ai-train", "ai_train"}:
        return cmd_ai_train(rest) if cmd_ai_train else _missing("ai-train")
    if cmd in {"pack-docs", "pack_docs"}:
        return cmd_pack_docs(rest) if cmd_pack_docs else _missing("pack-docs")
    if cmd == "docs":
        return cmd_docs(rest) if cmd_docs else _missing("docs")
    if cmd == "backlog":
        return cmd_backlog(rest) if cmd_backlog else _missing("backlog")
    if cmd == "bridge":
        return cmd_bridge(rest) if cmd_bridge else _missing("bridge")
    if cmd == "indicators":                                  # <— NEW
        return cmd_ind(rest) if cmd_ind else _missing("indicators")   # <— NEW

    print(f"Unknown command: {cmd}\n")
    print(HELP)
    return 2

if __name__ == "__main__":
    raise SystemExit(main())

