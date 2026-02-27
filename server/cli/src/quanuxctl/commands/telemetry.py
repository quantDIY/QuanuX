import typer
import asyncio
import json
from rich.console import Console

app = typer.Typer(help="Manage Node Telemetry Services remotely across the NATS mesh.")
console = Console()

async def send_telemetry_cmd(node: str, service: str, action: str):
    try:
        import nats
    except ImportError:
        console.print("[bold red]nats-py is not installed. Cannot communicate with the grid.[/bold red]")
        raise typer.Exit(1)
        
    try:
        nc = await nats.connect("nats://127.0.0.1:4222")
        subject = f"QUANUX.NODE.CMD.{node}"
        
        # Enforce strict JSON schema for IPC security
        ipc_msg = {
            "action": action,
            "service": service
        }
        payload = json.dumps(ipc_msg).encode('utf-8')
        
        console.print(f"Publishing JSON command '{action} {service}' to {subject}...")
        # In a real environment we might expect a reply, but for now we publish
        await nc.publish(subject, payload)
        await nc.flush()
        await nc.close()
        
        console.print("[bold green]Command dispatched successfully.[/bold green]")
    except Exception as e:
        console.print(f"[bold red]Failed to dispatch command: {e}[/bold red]")
        raise typer.Exit(1)


def run_async(coro):
    loop = asyncio.get_event_loop()
    loop.run_until_complete(coro)

@app.command("start")
def start_service(
    node: str = typer.Option(..., "--node", "-n", help="Target node hostname"),
    service: str = typer.Option(..., "--service", "-s", help="Service name: nerve or envoy")
):
    """Start a remote telemetry service."""
    run_async(send_telemetry_cmd(node, service, "start"))

@app.command("stop")
def stop_service(
    node: str = typer.Option(..., "--node", "-n", help="Target node hostname"),
    service: str = typer.Option(..., "--service", "-s", help="Service name: nerve or envoy")
):
    """Stop a remote telemetry service."""
    run_async(send_telemetry_cmd(node, service, "stop"))

@app.command("restart")
def restart_service(
    node: str = typer.Option(..., "--node", "-n", help="Target node hostname"),
    service: str = typer.Option(..., "--service", "-s", help="Service name: nerve or envoy")
):
    """Restart a remote telemetry service."""
    run_async(send_telemetry_cmd(node, service, "restart"))

@app.command("status")
def status_service(
    node: str = typer.Option(..., "--node", "-n", help="Target node hostname"),
    service: str = typer.Option(..., "--service", "-s", help="Service name: nerve or envoy")
):
    """Check the status of a remote telemetry service."""
    run_async(send_telemetry_cmd(node, service, "status"))
