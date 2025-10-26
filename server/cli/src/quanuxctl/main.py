#!/usr/bin/env python3
"""
QuanuX Control (quanuxctl) — scaffold dispatcher.
Safe to run even when some subcommands aren't implemented yet.
"""

import sys

HELP = """\
QuanuX Control (quanuxctl) — scaffold v0.0.1

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
"""

def main(argv=None) -> int:
    # Lazy imports so missing subcommands don't crash the dispatcher
    # Each import is wrapped with a fallback stub to keep things resilient.
    def _try_import(module, func):
        try:
            mod = __import__(module, fromlist=[func])
            return getattr(mod, func)
        except Exception:
            return None

    cmd_start     = _try_import("quanuxctl.commands.start", "cmd_start")
    cmd_build     = _try_import("quanuxctl.commands.build", "cmd_build")
    cmd_diag      = _try_import("quanuxctl.commands.diagnose", "cmd_diagnose")
    cmd_ai_train  = _try_import("quanuxctl.commands.ai_train", "cmd_ai_train")
    cmd_pack_docs = _try_import("quanuxctl.commands.pack_docs", "cmd_pack_docs")
    cmd_docs      = _try_import("quanuxctl.commands.docs", "cmd_docs")
    cmd_backlog   = _try_import("quanuxctl.commands.docs", "cmd_backlog")

    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in {"-h", "--help", "help"}:
        print(HELP)
        return 0

    cmd = argv[0]

    if cmd == "start":
        if cmd_start: cmd_start(argv[1:])
        else: print("start scaffold not available yet.")
    elif cmd == "build":
        if cmd_build: cmd_build(argv[1:])
        else: print("build scaffold not available yet.")
    elif cmd == "diagnose":
        if cmd_diag: cmd_diag(argv[1:])
        else: print("diagnose scaffold not available yet.")
    elif cmd in {"ai-train", "ai_train"}:
        if cmd_ai_train: cmd_ai_train(argv[1:])
        else: print("ai-train scaffold not available yet.")
    elif cmd in {"pack-docs", "pack_docs"}:
        if cmd_pack_docs: cmd_pack_docs(argv[1:])
        else: print("pack-docs scaffold not available yet.")
    elif cmd == "docs":
        if cmd_docs: cmd_docs(argv[1:])
        else: print("docs index not available yet.")
    elif cmd == "backlog":
        if cmd_backlog: cmd_backlog(argv[1:])
        else: print("backlog not available yet.")
    else:
        print(f"Unknown command: {cmd}\n")
        print(HELP)
        return 2
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
