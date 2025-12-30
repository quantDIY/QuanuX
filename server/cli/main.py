
import sys
import os

# Ensure repo root is on path so imports work
sys.path.append(os.getcwd())

from server.cli.src.quanuxctl.main import app

if __name__ == "__main__":
    app()
