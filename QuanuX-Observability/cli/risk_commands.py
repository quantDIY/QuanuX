import asyncio
import struct
import typer
from rich.console import Console
from rich.table import Table
import nats
from nats.errors import TimeoutError, NoRespondersError

app = typer.Typer(help="QuanuX Control Plane: Risk Kernel & Exposure Management")
console = Console()

async def connect_nats():
    # Attempt connection to JetStream mesh (assume local for CLI)
    try:
        nc = await nats.connect("nats://127.0.0.1:4222")
        js = nc.jetstream()
        return nc, js
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to connect to CNATS mesh: {e}")
        raise typer.Exit(code=1)

@app.command("view-state")
def view_state():
    """
    Subscribes to the NATS JetStream KV store to pull the real-time aggregated global exposure.
    Audits the execution plane without directly touching the C++ Sovereign Engine.
    """
    async def _view():
        nc, js = await connect_nats()
        try:
            # The Risk KV Bucket containing global states
            kv = await js.key_value("RISK_STATE")
            
            console.print("[cyan]Querying JetStream Global Risk Registry...[/cyan]")
            entry = await kv.get("global.notional.exposure")
            
            # Unpack the 64-bit integer standard from the C++ mesh
            if len(entry.value) == 8:
                current_notional = struct.unpack("<q", entry.value)[0]
                
                table = Table(title="Global Exposure State (JetStream KV)")
                table.add_column("Metric", style="magenta")
                table.add_column("Value (Implied 2 Decimal)", style="green")
                table.add_column("Revision", style="yellow")
                
                table.add_row("current_global_notional", f"{current_notional:,}", str(entry.revision))
                console.print(table)
            else:
                console.print("[bold red]ERROR:[/bold red] Invalid memory payload length detected in KV store.")
        except Exception as e:
            console.print(f"[bold red]ERROR:[/bold red] Failed to retrieve risk state. Is the KV bucket initialized? {e}")
        finally:
            await nc.close()
            
    asyncio.run(_view())

@app.command("update-cap")
def update_cap(new_cap: int = typer.Argument(..., help="New daily notional cap in cents (e.g., $1M = 100000000)")):
    """
    Publishes a new daily notional cap limit asynchronously to the execution mesh.
    Background threads in the C++ RiskKernel will catch this and apply it without blocking the hot path.
    """
    async def _update():
        nc, _ = await connect_nats()
        try:
            payload = struct.pack("<q", new_cap)
            await nc.publish("quanux.control.risk.cap", payload)
            
            console.print(f"[bold green]SUCCESS:[/bold green] Published new Notional Cap ({new_cap:,}) to JetStream mesh.")
            console.print("[dim]C++ Nodes will asynchronously hydrate this new limit into local RAM matrices.[/dim]")
        except Exception as e:
            console.print(f"[bold red]ERROR:[/bold red] Publish failed: {e}")
        finally:
            await nc.close()
            
    asyncio.run(_update())

@app.command("force-hydrate")
def force_hydrate(node_id: str = typer.Argument(..., help="The target node identifier (e.g., 'SFO-EXEC-01')")):
    """
    Commands a specific node to pause active execution, perform a blocking read from the JetStream KV global state, 
    and re-seed its local RAM matrix.
    """
    async def _hydrate():
        nc, _ = await connect_nats()
        try:
            subject = f"quanux.control.risk.hydrate.{node_id}"
            # Sending an empty payload as a trigger signal
            await nc.publish(subject, b"")
            console.print(f"[bold yellow]ACTION:[/bold yellow] Dispatched Hydration Gate trigger to [bold]{node_id}[/bold].")
            console.print("[dim]Target node will execute a synchronous block against the KV store before resuming.[/dim]")
        except Exception as e:
            console.print(f"[bold red]ERROR:[/bold red] Trigger sequence failed: {e}")
        finally:
            await nc.close()
            
    asyncio.run(_hydrate())

if __name__ == "__main__":
    app()
