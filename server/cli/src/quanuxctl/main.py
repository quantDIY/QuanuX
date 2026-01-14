#!/usr/bin/env python3
"""
QuanuX Control (quanuxctl) — v0.1.0
Unified Developer CLI.
"""
import typer
from rich.console import Console
from .commands import secrets, bridge, skills
from . import __version__

app = typer.Typer(
    name="quanuxctl",
    help="QuanuX Control CLI",
    add_completion=True,
    rich_markup_mode="rich",
    invoke_without_command=True
)
console = Console()

# Register subcommands
app.add_typer(secrets.app, name="secrets", help="Manage API keys and secrets via OS Keyring.")
app.add_typer(bridge.app, name="bridge", help="Manage the SignalR Bridge.")
app.add_typer(skills.app, name="skills", help="Manage and discover Agent Skills.")


@app.command()
def mcp():
    """
    Start the QuanuX MCP Server (Stdio Mode).
    Connect your Agent (VS Code/Cursor) to this command:
    'quanuxctl mcp'
    """
    import sys
    from server.mcp.server import mcp as mcp_server
    # Run the fastmcp server
    # FastMCP.run() by default handles sys.argv, but here we want to force run it directly
    # Since we are inside a typer command, we might need to be careful with args.
    # Actually, mcp.run() usually takes over.
    mcp_server.run()


@app.callback()
def main(
    version: bool = typer.Option(
        None,
        "--version",
        "-v",
        help="Show version and exit.",
        is_eager=True
    )
):
    if version:
        console.print(f"[bold green]QuanuX Control (quanuxctl)[/bold green] v{__version__}")
        raise typer.Exit()

def cli():
    """Entry point for the CLI."""
    app()

if __name__ == "__main__":
    cli()
