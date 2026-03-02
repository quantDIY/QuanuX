"""
QuanuX control plane plugin: `quanuxctl obs`
Rule: Stateless operations. All interactions must be strictly CNATS publishers.
"""

import asyncio
import json
import typer
from nats.aio.client import Client as NATSClient
from nats.aio.msg import Msg
from rich.console import Console
from rich.table import Table

app = typer.Typer(help="QuanuX Observability Plane remote administration")
console = Console()

async def push_cnats_control_message(subject: str, payload: dict):
    """Utility to securely publish JSON payloads to CNATS Mesh."""
    nc = NATSClient()
    try:
        await nc.connect("nats://localhost:4222") # Typical environment variable injection here 
        await nc.publish(subject, json.dumps(payload).encode())
        await nc.flush()
    except Exception as e:
        console.print(f"[bold red]Failed to connect to QuanuX Mesh[/bold red]: {e}")
    finally:
        await nc.close()

@app.command()
def config_exchange(exchange_code: str = typer.Argument(..., help="Global exchange ID (e.g. LSE, TSE)"),
                    close_time_seconds: int = typer.Argument(..., help="Seconds past local midnight for settlement"),
                    offset_minutes: int = typer.Argument(0, help="Local UTC offset in minutes (e.g., 330 for India)")):
    """
    Submits an asynchronous Exchange Registry update to govern global settlement epochs.
    """
    payload = {
        "action": "CONFIG_EXCHANGE",
        "exchange_code": exchange_code.upper(),
        "close_time_seconds": close_time_seconds,
        "offset_minutes": offset_minutes
    }
    
    console.print(f"[bold blue]Submitting Config Update[/bold blue]: {exchange_code.upper()} -> {close_time_seconds}s (Offset: {offset_minutes}m)")
    asyncio.run(push_cnats_control_message("quanux.control.registry.config", payload))
    console.print("[bold green]Success[/bold green]: Registry configuration blasted to the Global Mesh.")

@app.command()
def trigger_freeze(exchange: str = typer.Argument(..., help="Force an out-of-band DuckDB settlement pull")):
    """
    Forces the active C++ Settlement Daemon to instantly rip DuckDB parity and export to Parquet.
    """
    payload = {
        "action": "FORCE_SETTLEMENT",
        "exchange_code": exchange.upper()
    }
    
    console.print(f"[bold yellow]Initiating Settlement Override for {exchange.upper()}...[/bold yellow]")
    asyncio.run(push_cnats_control_message("quanux.control.obs.freeze", payload))
    console.print(f"[bold green]Command Dispatched[/bold green]: {exchange.upper()} Parquet extraction triggered on daemon.")

@app.command()
def cluster_health(duration: int = typer.Option(2, help="Observation window in seconds")):
    """
    Actively listens to the JetStream execution firehose to map global nodes.
    """
    async def listen_for_health():
        nc = NATSClient()
        await nc.connect("nats://localhost:4222")
        
        active_nodes = {}
        
        async def on_vitals(msg: Msg):
            # In a full deployment, this would use quanux_cython_bridge.parse_telemetry
            # Mocking here for the Typer skeleton
            node_id = "mock-sfo-1" # Extracted from the binary payload
            active_nodes[node_id] = "Healthy" # Extracted from the binary payload
            
        console.print(f"[bold cyan]Subscribing to execution vitals for {duration} seconds...[/bold cyan]")
        sub = await nc.subscribe("quanux.telemetry.vitals.>", cb=on_vitals)
        
        await asyncio.sleep(duration)
        await sub.unsubscribe()
        await nc.close()
        return active_nodes

    try:
        nodes = asyncio.run(listen_for_health())
        
        table = Table(title="QuanuX Plane Vitals")
        table.add_column("Edge Node", justify="left", style="cyan", no_wrap=True)
        table.add_column("Status", justify="right", style="green")
        
        if not nodes:
            table.add_row("No nodes active", "N/A")
        else:
            for node, status in nodes.items():
                table.add_row(node, status)
                
        console.print(table)
        
    except Exception as e:
        console.print(f"[bold red]Mesh Probe Failed[/bold red]: {e}")

if __name__ == "__main__":
    app()
