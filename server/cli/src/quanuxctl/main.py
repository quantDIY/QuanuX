"""
QuanuX CLI (scaffold)
Usage: quanuxctl <command> [options]
"""

from __future__ import annotations
import sys

from .commands.start import cmd_start
from .commands.build import cmd_build
from .commands.diagnose import cmd_diagnose
from .commands.ai_train import cmd_ai_train
from .commands.pack_docs import cmd_pack_docs

def app(argv: list[str] | None = None) -> None:
    argv = list(sys.argv[1:] if argv is None else argv)
    if not argv or argv[0] in {"-h", "--help", "help"}:
        print(
            "QuanuX CLI v0.0.1 (scaffold)\n"
            "Commands:\n"
            "  start        Start local dev services (stub)\n"
            "  build        Build artifacts for server/client/suite (stub)\n"
            "  diagnose     Run local diagnostics (stub)\n"
            "  ai-train     Prep/train AI agents on context (stub)\n"
            "  pack-docs    Bundle docs into zips (stub)\n"
            "\n"
            "Examples:\n"
            "  quanuxctl start --server\n"
            "  quanuxctl build --suite\n"
            "  quanuxctl diagnose\n"
        )
        return

    cmd = argv.pop(0)
    if cmd == "start":
        cmd_start(argv)
    elif cmd == "build":
        cmd_build(argv)
    elif cmd == "diagnose":
        cmd_diagnose(argv)
    elif cmd in {"ai-train", "ai_train"}:
        cmd_ai_train(argv)
    elif cmd in {"pack-docs", "pack_docs"}:
        cmd_pack_docs(argv)
    else:
        print(f"Unknown command: {cmd}\nUse 'quanuxctl --help' for commands.")
        sys.exit(1)
