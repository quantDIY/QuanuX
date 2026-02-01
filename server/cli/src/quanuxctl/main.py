#!/usr/bin/env python3
"""
QuanuX Control (quanuxctl) — v0.1.0
Unified Developer CLI.
"""
import typer
from rich.console import Console
from .commands import secrets, bridge, skills, extensions, integrate, node, storage, indicators, module, vcs, dashboard, topstepx, geminicli
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
app.add_typer(secrets.app, name="sec", help="Alias for secrets", hidden=True)

app.add_typer(bridge.app, name="bridge", help="Manage the SignalR Bridge.")
app.add_typer(bridge.app, name="b", help="Alias for bridge", hidden=True)

app.add_typer(skills.app, name="skills", help="Manage and discover Agent Skills.")
app.add_typer(skills.app, name="k", help="Alias for skills", hidden=True)

app.add_typer(extensions.app, name="ext", help="Manage QXP Extensions (List, Run).")
app.add_typer(extensions.app, name="e", help="Alias for ext", hidden=True)

app.add_typer(node.app, name="node", help="Manage Remote Execution Nodes (Deploy, List).")
app.add_typer(node.app, name="n", help="Alias for node", hidden=True)

app.add_typer(storage.app, name="storage", help="Manage connected storage and databases.")
app.add_typer(storage.app, name="s", help="Alias for storage", hidden=True)

app.add_typer(indicators.app, name="indicators", help="Manage Indicator Registry (Community).")
app.add_typer(indicators.app, name="i", help="Alias for indicators", hidden=True)

app.add_typer(module.app, name="module", help="Manage Core Modules (Lifecycle).")
app.add_typer(module.app, name="m", help="Alias for module", hidden=True)

app.add_typer(vcs.app, name="vcs", help="Version Control System Connectors.")
app.add_typer(vcs.app, name="vcs", help="Version Control System Connectors.")
app.add_typer(vcs.app, name="v", help="Alias for vcs", hidden=True)

app.add_typer(dashboard.app, name="dashboard", help="Launch System Dashboard (TUI).")
app.add_typer(dashboard.app, name="d", help="Alias for dashboard", hidden=True)

app.add_typer(topstepx.app, name="topstepx", help="Manage TopstepX Extension.")
app.add_typer(topstepx.app, name="ts", help="Alias for topstepx", hidden=True)

app.add_typer(geminicli.app, name="geminicli", help="Manage Gemini CLI Integration.")
app.add_typer(geminicli.app, name="geminicli", help="Manage Gemini CLI Integration.")
app.add_typer(geminicli.app, name="gemini", help="Alias for geminicli", hidden=True)

# Top-level aliases for common extension operations
# Top-level aliases for common extension operations
# cli.add_command(integrate.integrate) # REMOVED: Broken and redundant. Use 'quanuxctl ext integrate'
@app.command("install")
def install(name: str, version: str = typer.Option(None, "--version", "-v")):
    """Install a QuanuX extension (Alias for 'ext install')."""
    extensions.install_extension(name, version)

@app.command("remove")
def remove(name: str, force: bool = typer.Option(False, "--force", "-f")):
    """Remove a QuanuX extension (Alias for 'ext remove')."""
    extensions.remove_extension(name, force)

@app.command("upgrade")
def upgrade(name: str):
    """Auto-upgrade extension (Alias for 'ext upgrade')."""
    extensions.upgrade(name)

@app.command("upgradeable")
def upgradeable(name: str):
    """Check for updates (Alias for 'ext upgradeable')."""
    extensions.upgradeable(name)


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
