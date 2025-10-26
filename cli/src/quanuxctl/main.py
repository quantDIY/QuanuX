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

def main(argv=None):
    from .commands.start import cmd_start
    from .commands.pack_docs import cmd_pack_docs  # safe even if stub
    try:
        from .commands.docs import cmd_docs, cmd_backlog
    except Exception:
        # allow running before docs command exists
        def cmd_docs(_): print("Docs index not present yet.")
        def cmd_backlog(_): print("Backlog not present yet.")

    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in {"-h", "--help", "help"}:
        print(HELP)
        return 0

    cmd = argv[0]
    if cmd == "start":
        cmd_start(argv[1:])
    elif cmd == "build":
        print("Build is a scaffold. Later this will package Suite artifacts.")
    elif cmd == "diagnose":
        print("Diagnose is a scaffold. Later this will check env/tooling.")
    elif cmd in {"pack-docs", "pack_docs"}:
        cmd_pack_docs(argv[1:])
    elif cmd == "docs":
        cmd_docs(argv[1:])
    elif cmd == "backlog":
        cmd_backlog(argv[1:])
    elif cmd == "ai-train":
        print("AI-train scaffold. Later this will index /meta/docs + /meta/context.")
    else:
        print(f"Unknown command: {cmd}\n")
        print(HELP)
        return 2
    return 0
