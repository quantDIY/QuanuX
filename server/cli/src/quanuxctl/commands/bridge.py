import typer
import os
import subprocess
import sys
import shlex
from pathlib import Path
from rich.console import Console

app = typer.Typer(help="Manage the SignalR Bridge (Topstep connectivity).")
console = Console()

# Path to repo root (assuming THIS file is in server/cli/src/quanuxctl/commands)
# server/cli/src/quanuxctl/commands -> server/cli/src/quanuxctl -> server/cli/src -> server/cli -> server -> ROOT
ROOT = Path(__file__).resolve().parents[5] 
# Actually, let's rely on server module path if possible, or relative path safe calculation.
# /Users/Duncan/Antigravity/QuanuX/QuanuX/server/cli/src/quanuxctl/commands/bridge.py
# parents[0] = commands
# parents[1] = quanuxctl
# parents[2] = src
# parents[3] = cli
# parents[4] = server
# parents[5] = QuanuX (repo root)

# Verify correct path by checking for 'server/bridges'
SERVER_DIR = ROOT / "server"
if not (SERVER_DIR / "bridges").exists():
    # Fallback/Debug
    SERVER_DIR = Path(os.getcwd()) / "server"

FLASK_APP = SERVER_DIR / "bridges" / "signalr" / "flask" / "app.py"
NODE_APP  = SERVER_DIR / "bridges" / "signalr" / "node" / "index.mjs"

@app.command()
def start(
    runtime: str = typer.Option("flask", "--runtime", "-r", help="Runtime to use: 'flask' or 'node'."),
    port: int = typer.Option(8077, "--port", "-p", help="Port to bind the bridge to.")
):
    """
    Start the SignalR bridge process.
    """
    console.print(f"[bold blue]Starting SignalR Bridge ({runtime}) on port {port}...[/bold blue]")
    
    os.environ["QUANUX_BRIDGE_PORT"] = str(port)
    
    cmd = ""
    if runtime == "flask":
        cmd = f"python3 {FLASK_APP}"
    elif runtime == "node":
        cmd = f"node {NODE_APP}"
    else:
        console.print(f"[bold red]Unknown runtime: {runtime}[/bold red]")
        raise typer.Exit(code=1)
    
    try:
        # We run it synchronously for now, or use subprocess to spawn
        # If user wants it in background, they might use '&' in shell, 
        # or we could use Popen. For 'quanuxctl start', blocking is usually fine 
        # unless it's 'start-daemon'.
        subprocess.run(shlex.split(cmd), check=True)
    except KeyboardInterrupt:
        console.print("\n[yellow]Bridge stopped by user.[/yellow]")
    except Exception as e:
        console.print(f"[bold red]Error running bridge: {e}[/bold red]")
        raise typer.Exit(code=1)

@app.command()
def stop():
    """
    Stop the SignalR bridge (placeholder).
    """
    console.print("[yellow]Stop command is a placeholder (for now, Ctrl+C the running process).[/yellow]")
