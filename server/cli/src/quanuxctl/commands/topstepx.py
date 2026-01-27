import typer
import os
import sys
from rich.console import Console
from typing import Optional
from server.security.secrets import SecretsBroker, KeyringBackend
from .extensions import find_extension_path, install_extension, uninstall_extension

app = typer.Typer(help="Manage TopstepX Extension.")
console = Console()

EXTENSION_NAME = "topstepx"

def get_broker():
    return SecretsBroker(primary=KeyringBackend(), fallback=[])

@app.command()
def install():
    """Install TopstepX extension dependencies."""
    install_extension(EXTENSION_NAME)

@app.command()
def remove():
    """Remove TopstepX extension artifacts."""
    uninstall_extension(EXTENSION_NAME)

@app.command("user")
def set_user(username: str):
    """Set TopstepX Username."""
    broker = get_broker()
    broker.set("QUANUX_TOPSTEP__USERNAME", username)
    console.print(f"[green]Set TopstepX Username to: {username}[/green]")

@app.command("password")
def set_password(password: str):
    """Set TopstepX Password."""
    broker = get_broker()
    broker.set("QUANUX_TOPSTEP__PASSWORD", password)
    console.print(f"[green]Set TopstepX Password.[/green]")

@app.command("apikey")
def set_apikey(key: str):
    """Set TopstepX API Key."""
    broker = get_broker()
    broker.set("QUANUX_TOPSTEP__API_KEY", key)
    console.print(f"[green]Set TopstepX API Key.[/green]")

@app.command("user-hub")
def set_user_hub(url: str):
    """Set SignalR User Hub URL."""
    broker = get_broker()
    broker.set("QUANUX_SIGNALR_USER_HUB", url)
    console.print(f"[green]Set SignalR User Hub URL to: {url}[/green]")

@app.command("market-hub")
def set_market_hub(url: str):
    """Set SignalR Market Hub URL."""
    broker = get_broker()
    broker.set("QUANUX_SIGNALR_MARKET_HUB", url)
    console.print(f"[green]Set SignalR Market Hub URL to: {url}[/green]")

@app.command("env")
def show_env():
    """Show TopstepX Environment Variables (secrets masked)."""
    broker = get_broker()
    keys = [
        "QUANUX_TOPSTEP__USERNAME",
        "QUANUX_TOPSTEP__PASSWORD",
        "QUANUX_TOPSTEP__API_KEY",
        "QUANUX_TOPSTEP__BASE_API_URL",
        "QUANUX_SIGNALR_USER_HUB",
        "QUANUX_SIGNALR_MARKET_HUB"
    ]
    
    console.print("[bold]TopstepX Environment:[/bold]")
    for k in keys:
        val = broker.get(k)
        if val:
            if "PASSWORD" in k or "KEY" in k:
                masked = val[:2] + "*" * (len(val) - 4) + val[-2:] if len(val) > 4 else "****"
                console.print(f"  {k}: [green]{masked}[/green]")
            else:
                console.print(f"  {k}: [green]{val}[/green]")
        else:
            console.print(f"  {k}: [red]Not Set[/red]")

    # Check extension path
    path = find_extension_path(EXTENSION_NAME)
    if path:
        console.print(f"\n[bold]Extension Location:[/bold] {path}")
    else:
        console.print(f"\n[red]Extension not found in extensions directory![/red]")
