import typer
from rich.console import Console
import subprocess
import os

from .infra_commands import check_provider

app = typer.Typer(help="Manage QuanuX Edge Engine Tuning & Execution")
console = Console()

def get_ansible_dir():
    current_dir = os.path.abspath(os.path.dirname(__file__))
    repo_root = os.path.abspath(os.path.join(current_dir, "../../../../../"))
    possible_paths = [
        os.path.join(repo_root, "QuanuX-Infra/ansible"),
        os.path.expanduser("~/Antigravity/QuanuX/QuanuX/QuanuX-Infra/ansible")
    ]
    for p in possible_paths:
        if os.path.exists(p) and os.path.isdir(p):
            return os.path.abspath(p)
    return None

@app.command("tune")
def engine_tune(target: str = typer.Option("gcp", help="Infrastructure target (do or gcp)")):
    """Invokes kernel and OS tuning playbooks for the designated architecture."""
    check_provider(target)
    ansible_dir = get_ansible_dir()
    if not ansible_dir:
        console.print("[red]Could not locate QuanuX-Infra/ansible directory.[/red]")
        raise typer.Exit(1)
        
    if target.lower() == "gcp":
        console.print("[bold blue]Initiating GCP Engine Tuning (c2-standard profile)...[/bold blue]")
        env = os.environ.copy()
        env["QUANUX_TARGET"] = "gcp"
        playbook = "06-gcp-engine-tune.yml"
        res = subprocess.run(["ansible-playbook", "-i", "dynamic_inventory.py", playbook], cwd=ansible_dir, env=env)
        if res.returncode != 0:
            console.print(f"[bold red]FATAL:[/bold red] Playbook {playbook} failed.")
            raise typer.Exit(1)
    else:
        console.print("[dim]DO Engine Tuning not implemented in Phase 4 target matrix.[/dim]")

@app.command("start")
def engine_start(target: str = typer.Option("gcp", help="Infrastructure target (do or gcp)")):
    """Initializes CNATS, Annex, and Omega Edge pipelines."""
    check_provider(target)
    ansible_dir = get_ansible_dir()
    if not ansible_dir:
        console.print("[red]Could not locate QuanuX-Infra/ansible directory.[/red]")
        raise typer.Exit(1)
        
    if target.lower() == "gcp":
        console.print("[bold blue]Initiating GCP Engine Start (Annex & Omega C++ Decoder)...[/bold blue]")
        env = os.environ.copy()
        env["QUANUX_TARGET"] = "gcp"
        playbook = "07-gcp-engine-start.yml"
        res = subprocess.run(["ansible-playbook", "-i", "dynamic_inventory.py", playbook], cwd=ansible_dir, env=env)
        if res.returncode != 0:
            console.print(f"[bold red]FATAL:[/bold red] Playbook {playbook} failed.")
            raise typer.Exit(1)
        console.print("[dim]DO Engine Start not implemented in Phase 4 target matrix.[/dim]")

@app.command("setup-topology")
def engine_setup_topology(target: str = typer.Option("gcp", help="Infrastructure target (do or gcp)")):
    """Configures the core JetStream QUANUX_INGEST stream and dual-consumer routing topology."""
    check_provider(target)
    
    # We must resolve the Annex Node IP dynamically to connect to the NATS Fabric.
    # In full production, this would hit the Panopticon NATS endpoints.
    ansible_dir = get_ansible_dir()
    if not ansible_dir:
        console.print("[red]Could not locate QuanuX-Infra/ansible directory.[/red]")
        raise typer.Exit(1)
        
    import sys
    sys.path.insert(0, ansible_dir)
    try:
        from dynamic_inventory import build_inventory
        inventory = build_inventory()
    except Exception as e:
        console.print(f"[red]Failed to resolve dynamic target inventory: {e}[/red]")
        raise typer.Exit(1)
        
    # Find the Annex node public IP
    annex_pub_ip = None
    try:
        hosts = inventory.get("quanux_annex_node", {}).get("hosts", [])
        if hosts:
            annex_pub_ip = inventory["_meta"]["hostvars"][hosts[0]].get("ansible_host")
    except Exception:
        pass

    if not annex_pub_ip:
        console.print(f"[red]Could not dynamically resolve the public IP for quanux_annex_node.[/red]")
        raise typer.Exit(1)
        
    console.print(f"[bold blue]Targeting NATS Fabric at: {annex_pub_ip}[/bold blue]")
    
    nats_url = f"nats://{annex_pub_ip}:4222"
    
    stream_name = "QUANUX_INGEST"
    subjects = "VENUE.*.RAW.>"
    
    # 1. Add/Update the Core Ingestion Stream (File backed, 50GB max size retention)
    console.print(f"[dim]Ensuring Stream: {stream_name} (Storage: File, Max: 50GB, Subjects: {subjects})...[/dim]")
    add_stream_cmd = [
        "nats", "stream", "add", stream_name,
        "--subjects", subjects,
        "--storage", "file",
        "--retention", "limits",
        "--max-bytes", "50G", # 50GB Limits-based retention
        "--server", nats_url
    ]
    
    try:
        # Popen to bypass interactive prompts if stream exists
        res = subprocess.run(add_stream_cmd, input=b'\n\n', capture_output=True)
        if res.returncode != 0 and b"already in use" not in res.stderr:
             console.print(f"[yellow]Stream configuration returned state: {res.stderr.decode().strip()}[/yellow]")
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to provision stream: {e}")
        raise typer.Exit(1)
        
    # 2. Add Consumer A (Annex Hot Path)
    consumer_a = "ANNEX_PROCESSOR"
    console.print(f"[dim]Ensuring Consumer A: {consumer_a} (Durable, Pull, Explicit Ack)...[/dim]")
    add_cons_a_cmd = [
        "nats", "consumer", "add", stream_name, consumer_a,
        "--pull", "--durable", consumer_a,
        "--ack", "explicit",
        "--server", nats_url
    ]
    
    try:
        res = subprocess.run(add_cons_a_cmd, input=b'\n\n', capture_output=True)
        if res.returncode != 0 and b"already exists" not in res.stderr:
             console.print(f"[yellow]Consumer {consumer_a} warning: {res.stderr.decode().strip()}[/yellow]")
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to provision Consumer A: {e}")
        raise typer.Exit(1)
        
    # 3. Add Consumer B (Aleph Hot Path)
    consumer_b = "ALEPH_OBSERVER"
    console.print(f"[dim]Ensuring Consumer B: {consumer_b} (Durable, Pull, Explicit Ack)...[/dim]")
    add_cons_b_cmd = [
        "nats", "consumer", "add", stream_name, consumer_b,
        "--pull", "--durable", consumer_b,
        "--ack", "explicit",
        "--server", nats_url
    ]
    
    try:
        res = subprocess.run(add_cons_b_cmd, input=b'\n\n', capture_output=True)
        if res.returncode != 0 and b"already exists" not in res.stderr:
             console.print(f"[yellow]Consumer {consumer_b} warning: {res.stderr.decode().strip()}[/yellow]")
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to provision Consumer B: {e}")
        raise typer.Exit(1)
        
    console.print("[bold green]SUCCESS:[/bold green] JetStream Line-Rate Forking Topology is actively deployed.")
    console.print("[dim]Run 'nats stream info QUANUX_INGEST' externally to verify cluster attachment.[/dim]")
