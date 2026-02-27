import typer
import asyncio
import json
import os
import yaml
from pathlib import Path
from rich.console import Console

console = Console()
app = typer.Typer()

try:
    import nats
    from nats.errors import TimeoutError
except ImportError:
    pass

import hashlib
import subprocess

def compute_local_hash(filepath: str):
    sha256_hash = hashlib.sha256()
    with open(filepath, "rb") as f:
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()

def load_capability_registry():
    registry_path = Path("QuanuX-Clustering/manifests/capability_registry.yaml")
    if not registry_path.exists():
        console.print("[red]Error: Capability Registry not found.[/red]")
        raise typer.Exit(1)
    with open(registry_path, "r") as f:
        return yaml.safe_load(f).get("capabilities", {})

async def do_predeploy(payload_path: str, target: str):
    req_file = Path(payload_path) / "requirements.json"
    if not req_file.exists():
        console.print(f"[red]Error: {req_file} missing. Cannot verify handshake.[/red]")
        raise typer.Exit(1)

    with open(req_file, "r") as f:
        req_data = json.load(f)
    
    requires = req_data.get("requires", [])
    
    # 1. Validate requirements against registry
    registry = load_capability_registry()
    for req in requires:
        if req not in registry:
            console.print(f"[red]FATAL: Capability '{req}' is not registered in the Central Registry.[/red]")
            raise typer.Exit(1)

    console.print(f"[cyan]Executing Habitat Interrogation to {target}...[/cyan]")
    try:
        nc = await nats.connect("nats://127.0.0.1:4222")
        subject = f"QUANUX.NODE.HANDSHAKE.{target}"
        
        ipc_msg = json.dumps({"action": "predeploy", "requires": requires}).encode('utf-8')
        
        # Request-Reply pattern
        response = await nc.request(subject, ipc_msg, timeout=5.0)
        resp_data = json.loads(response.data.decode())
        
        if resp_data.get("status") == "ACK":
            console.print(f"[green]Handshake ACK. Habitat is compatible with {requires}.[/green]")
            await nc.close()
            return True, registry
        else:
            console.print(f"[red]Handshake NACK. Habitat incompatible. DNA: {resp_data.get('habitat')}[/red]")
            await nc.close()
            return False, registry

    except TimeoutError:
        console.print(f"[red]Timeout Handshake: Target {target} unresponsive.[/red]")
        if 'nc' in locals(): await nc.close()
        return False, registry
    except Exception as e:
        console.print(f"[red]NATS Error: {e}[/red]")
        if 'nc' in locals(): await nc.close()
        return False, registry

@app.command("predeploy")
def predeploy(payload: str = typer.Option(..., "--payload", "-p", help="Path to payload directory"),
              target: str = typer.Option(..., "--target", "-t", help="Target Node Hostname")):
    """Performs a Capability Handshake prior to deployment."""
    # Typer async wrapper
    loop = asyncio.get_event_loop()
    success, _ = loop.run_until_complete(do_predeploy(payload, target))
    if not success:
        raise typer.Exit(1)

@app.command("deploy")
def deploy(payload: str = typer.Option(..., "--payload", "-p", help="Path to payload directory"),
           target: str = typer.Option(..., "--target", "-t", help="Target Node Hostname")):
    """Deploys a payload into the outer shell (Habitat) safely using valid wiring hooks."""
    loop = asyncio.get_event_loop()
    success, registry = loop.run_until_complete(do_predeploy(payload, target))
    
    if not success:
        console.print("[red]Deployment Aborted due to failed handshake.[/red]")
        raise typer.Exit(1)

    console.print("[green]Handshake verified. Proceeding with deployment...[/green]")
    
    # Read requirements to build the execution string
    req_file = Path(payload) / "requirements.json"
    with open(req_file, "r") as f:
        requires = json.load(f).get("requires", [])

    wiring_prefixes = []
    for req in requires:
        hook = registry[req].get("wiring_hook", "")
        if hook:
            wiring_prefixes.append(hook)

    execution_prefix = " ".join(wiring_prefixes)
    console.print(f"[yellow]Wiring Execution Command: {execution_prefix} ./{Path(payload).name}_binary[/yellow]")
    
    # Send OOB Transfer Request
    binary_name = f"{Path(payload).name}_binary"
    binary_path = Path(payload) / binary_name
    
    # For simulation, if binary doesn't exist, we mock a hash
    file_hash = compute_local_hash(str(binary_path)) if binary_path.exists() else "mock_sha256_hash_if_binary_absent"
    mock_url = f"http://tier1-server:8080/payloads/{binary_name}"

    async def send_deploy():
        nc = await nats.connect("nats://127.0.0.1:4222")
        subject = f"QUANUX.NODE.DEPLOY.{target}"
        ipc_msg = json.dumps({
            "action": "deploy", 
            "url": mock_url, 
            "hash": file_hash, 
            "payload_name": Path(payload).name,
            "execution_prefix": execution_prefix
        }).encode('utf-8')
        await nc.publish(subject, ipc_msg)
        await nc.flush()
        await nc.close()

    loop.run_until_complete(send_deploy())
    console.print("[green]OOB Payload Transfer Signaled. Awaiting Native Envoy processing...[/green]")

@app.command("ssh")
def ssh_execute(target: str = typer.Argument(..., help="Target Node Hostname"),
                payload_name: str = typer.Argument(..., help="Name of the payload to execute")):
    """Executes a payload via SSH, conditioned on the C++ Envoy generating the wrapper."""
    wrapper_path = f"/opt/quanux/payloads/.run_{payload_name}.sh"
    console.print(f"[cyan]Initiating Conditioned SSH Execution against {target}...[/cyan]")
    
    ssh_cmd = ["ssh", f"{os.getenv('USER', 'quanux')}@{target}", wrapper_path]
    try:
        # We use subprocess.call to stream stdout/stderr back to the architect
        ret = subprocess.call(ssh_cmd)
        if ret != 0:
            console.print(f"[red]SSH Execution Failed (Code {ret}). Handshake likely NACK'd or connection failed.[/red]")
            raise typer.Exit(1)
        console.print("[green]Payload Execution Complete.[/green]")
    except Exception as e:
        console.print(f"[red]SSH Error: {e}[/red]")
        raise typer.Exit(1)
