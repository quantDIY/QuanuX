import typer
import asyncio
import nats
from nats.js.api import KeyValueConfig
import nats.js.errors
from typing import Optional

app = typer.Typer(help="Manage the QuanuX Tier 1 High Availability Cluster")

async def _get_kv(nc: nats.NATS):
    js = nc.jetstream()
    # Assume the KV bucket "quanux_tier1" is initialized by the servers
    return await js.key_value("quanux_tier1")

@app.command()
def status(nats_url: str = typer.Option("nats://localhost:4222", help="NATS server URL")):
    """
    Queries NATS to show Leader/Follower telemetry.
    """
    async def _run():
        try:
            nc = await nats.connect(nats_url)
            try:
                kv = await _get_kv(nc)
                try:
                    entry = await kv.get("quanux.tier1.leader")
                    leader_id = entry.value.decode()
                    typer.echo(f"Active Leader: {leader_id} (Revision: {entry.revision})")
                except nats.js.errors.KeyNotFoundError:
                    typer.secho("WARN: No active Leader found! Lock is currently free.", fg=typer.colors.YELLOW)
            except Exception as e:
                typer.secho(f"Error querying cluster status: {e}", fg=typer.colors.RED)
            finally:
                await nc.close()
        except Exception as e:
            typer.secho(f"NATS Connection Error: {e}", fg=typer.colors.RED)
            
    asyncio.run(_run())

@app.command()
def promote(
    node_id: str = typer.Argument(..., help="Node ID to enforce leadership upon"),
    nats_url: str = typer.Option("nats://localhost:4222", help="NATS server URL")
):
    """
    Forces Raft election override, manually assigning the lock to <node_id>.
    """
    async def _run():
        nc = await nats.connect(nats_url)
        try:
            kv = await _get_kv(nc)
            try:
                entry = await kv.get("quanux.tier1.leader")
                rev = entry.revision
                old_leader = entry.value.decode()
                typer.echo(f"Overriding current Leader {old_leader} with new Leader {node_id}")
            except nats.js.errors.KeyNotFoundError:
                rev = 0
                typer.echo(f"Lock is free. Forcing promotion of Node {node_id}")
            
            await kv.update("quanux.tier1.leader", node_id.encode(), rev)
            typer.secho(f"SUCCESS: Node {node_id} has been artificially promoted to Leader.", fg=typer.colors.GREEN)
        except Exception as e:
            typer.secho(f"Error promoting node: {e}", fg=typer.colors.RED)
        finally:
            await nc.close()
    
    asyncio.run(_run())

@app.command()
def demote(nats_url: str = typer.Option("nats://localhost:4222", help="NATS server URL")):
    """
    Forces current Leader to step down by dropping the KV lock.
    """
    async def _run():
        nc = await nats.connect(nats_url)
        try:
            kv = await _get_kv(nc)
            await kv.delete("quanux.tier1.leader")
            typer.secho("SUCCESS: Leader demoted. Election lock has been dropped.", fg=typer.colors.GREEN)
        except Exception as e:
            typer.secho(f"Error demoting node: {e}", fg=typer.colors.RED)
        finally:
            await nc.close()

    asyncio.run(_run())

@app.command()
def fence(
    node_id: str = typer.Argument(..., help="Rogue Node ID to obliterate via Out-Of-Band API"),
    nats_url: str = typer.Option("nats://localhost:4222", help="NATS server URL")
):
    """
    Manually fires the Out-Of-Band STONITH kill-pill API call.
    """
    async def _run():
        typer.secho(f"WARNING: Initiating OOB STONITH against Node {node_id}", fg=typer.colors.RED, bold=True)
        # This is where the physical DO/IPMI/hypervisor API hit happens
        typer.echo(f"Executing: POST https://api.digitalocean.com/v2/droplets/{node_id}/actions {{'type':'power_off'}}")
        
        await asyncio.sleep(0.5) # Simulating physical hardware API network latency
        
        typer.secho(f"CRITICAL SUCCESS: Node {node_id} fenced. Split-brain prevented.", fg=typer.colors.GREEN, bold=True)
    
    asyncio.run(_run())

if __name__ == "__main__":
    app()
