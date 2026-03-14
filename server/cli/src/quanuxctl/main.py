#!/usr/bin/env python3
"""
QuanuX Control (quanuxctl) — v0.1.0
Unified Developer CLI.
"""
import typer
from rich.console import Console
from .commands import secrets, bridge, skills, extensions, integrate, node, storage, indicators, module, vcs, dashboard, topstepx, geminicli, foundry, crucible, spreader, probe, telemetry, deploy, orchestra
from . import __version__

import os
import sys

# Dynamically link the decoupled QuanuX-Infra CLI tools and QuanuX root
infra_cli_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../QuanuX-Infra/cli"))
quanux_root_path = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../.."))

if infra_cli_path not in sys.path:
    sys.path.insert(0, infra_cli_path)
if quanux_root_path not in sys.path:
    sys.path.insert(0, quanux_root_path)

import habitat_commands
import nest_commands
import infra_commands

app = typer.Typer(
    name="quanuxctl",
    help="QuanuX Control CLI",
    add_completion=True,
    rich_markup_mode="rich",
    invoke_without_command=True
)
console = Console()

# Register subcommands
app.add_typer(habitat_commands.app, name="habitat", help="Manage QuanuX OS-level Habitat Provisioning.")
app.add_typer(nest_commands.app, name="nest", help="Manage QuanuX C++ Sovereign Engine Nest Deployment.")
app.add_typer(infra_commands.app, name="infra", help="QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning.")

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

app.add_typer(foundry.app, name="foundry", help="Manage QuanuX Foundry (Strategy Generator).")
app.add_typer(foundry.app, name="f", help="Alias for foundry", hidden=True)

app.add_typer(crucible.app, name="crucible", help="Manage QuanuX Crucible (Backtesting Studio).")
app.add_typer(crucible.app, name="c", help="Alias for crucible", hidden=True)

app.add_typer(spreader.app, name="spreader", help="Manage QuanuX-Spreader Engine Deployment and Strategy Injection.")

app.add_typer(orchestra.app, name="orchestra", help="Manage QuanuX-Orchestra Universal Naming Registry.")
app.add_typer(orchestra.app, name="o", help="Alias for orchestra", hidden=True)

app.add_typer(probe.app, name="probe", help="Neural Tap Diagnostic & Auto-Suture")

app.add_typer(telemetry.app, name="telemetry", help="Manage Node Telemetry Services remotely.")
app.add_typer(telemetry.app, name="t", help="Alias for telemetry", hidden=True)

app.add_typer(deploy.app, name="lifecycle", help="Dynamic Habitat Deployment & Telemetry Lifecycle")
# Standard shortcut command mapping
@app.command("predeploy")
def predeploy_alias(payload: str = typer.Option(..., "--payload", "-p"), target: str = typer.Option(..., "--target", "-t"), payload_type: str = typer.Option("extension", "--type", "-ty")):
    """Performs a Capability Handshake prior to deployment."""
    deploy.predeploy(payload, target, payload_type)

@app.command("deploy")
def deploy_alias(payload: str = typer.Option(..., "--payload", "-p"), target: str = typer.Option(..., "--target", "-t"), payload_type: str = typer.Option("extension", "--type", "-ty")):
    """Deploys a payload into the outer shell (Habitat) safely using valid wiring hooks."""
    deploy.deploy(payload, target, payload_type)

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

@app.command("enable")
def enable(feature: str):
    """Enable advanced functionality toggles."""
    if feature.lower() == "jaques-mode":
        import os
        config_path = os.path.expanduser("~/.quanux_jaques_mode")
        with open(config_path, "w") as f:
            f.write("1")
        console.print("[bold yellow]I know a couple of guys who are hoping to fire up some big Ambassadors at 11:59.[/bold yellow]")
    else:
        console.print(f"Unknown feature: {feature}")

@app.command("disable")
def disable(feature: str):
    """Disable advanced functionality toggles."""
    if feature.lower() == "jaques-mode":
        import os
        config_path = os.path.expanduser("~/.quanux_jaques_mode")
        if os.path.exists(config_path):
            os.remove(config_path)
        console.print("[bold green]Jacques-Mode disabled. Return to standard compliance.[/bold green]")
    else:
        console.print(f"Unknown feature: {feature}")


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
    ),
    ssh: str = typer.Option(
        None,
        "--ssh",
        "-ssh",
        help="Execute payload via Conditioned SSH: quanuxctl -ssh <nodeName> <payload_name>"
    )
):
    if version:
        console.print(f"[bold green]QuanuX Control (quanuxctl)[/bold green] v{__version__}")
        raise typer.Exit()
        
    if ssh:
        import sys
        # Parse payload name from remaining args or prompt
        if len(sys.argv) > 3:
            payload_name = sys.argv[3]
            deploy.ssh_execute(ssh, payload_name)
        else:
            console.print("[red]Missing payload name for -ssh execution.[/red]")
        raise typer.Exit()

def cli():
    """Entry point for the CLI."""
    app()

if __name__ == "__main__":
    cli()
