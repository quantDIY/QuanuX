import typer
from rich.console import Console
import subprocess
import os

app = typer.Typer(help="QuanuX Control Plane: Habitat Provisioning")
console = Console()

@app.command("equip")
def equip(target: str = typer.Argument(..., help="Ansible inventory target (e.g., edge_nodes)")):
    """
    Equips a raw bare-metal server with the QuanuX OS-level network boundaries 
    and native C++ toolchains. Leaves the server ready to receive a logic Nest.
    """
    console.print(f"[bold cyan]Initiating Habitat Protocol for:[/bold cyan] {target}")
    try:
        ansible_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../../QuanuX-Infra/ansible"))
        playbook_path = os.path.join(ansible_dir, "03a-habitat-equip.yml")
        
        cmd = [
            "ansible-playbook",
            "-i", "dynamic_inventory.py",
            playbook_path,
            "--limit", target
        ]
        
        console.print(f"[dim]Executing: {' '.join(cmd)}[/dim]")
        subprocess.run(cmd, cwd=ansible_dir, check=True)
        console.print("[bold green]Success:[/bold green] Habitat soil conditioned. Node is ready for Nests.")
    except subprocess.CalledProcessError as e:
        console.print("[bold red]Habitat Equip Failed.[/bold red] Playbook execution aborted.")
        raise typer.Exit(code=1)

@app.command("observe")
def observe(target: str = typer.Argument("all", help="Ansible inventory target (e.g., panopticon_ledger)")):
    """
    Deploys the Panopticon Observability Stack (Ledger, Buffer, and Shadow Node).
    Expects QUANUX_OS_PASS in the environment for OpenSearch native security.
    """
    console.print(f"[bold cyan]Initiating Observability Protocol for:[/bold cyan] {target}")
    try:
        ansible_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../../QuanuX-Infra/ansible"))
        playbook_path = os.path.join(ansible_dir, "02-panopticon-observability.yml")
        
        cmd = [
            "ansible-playbook",
            "-i", "dynamic_inventory.py",
            playbook_path,
            "--limit", target,
            "-e", "opensearch_admin_password={{ lookup('env', 'QUANUX_OS_PASS') }}"
        ]
        
        console.print(f"[dim]Executing: {' '.join(cmd)}[/dim]")
        subprocess.run(cmd, cwd=ansible_dir, check=True)
        console.print("[bold green]Success:[/bold green] Panopticon Observability Matrix activated.")
    except subprocess.CalledProcessError:
        console.print("[bold red]Observability Deployment Failed.[/bold red] Playbook execution aborted; verify QUANUX_OS_PASS is exported.")
        raise typer.Exit(code=1)

@app.command("sentinel")
def sentinel(
    target: str = typer.Argument(..., help="Ansible inventory target (e.g., quanux_annex_node)"),
    telemetry_interval: str = typer.Option("15s", "--telemetry-interval", help="Telemetry reporting interval for Telegraf")
):
    """
    Deploys the strict Low-Overhead Sentinel Protocol (Telegraf & Promtail).
    """
    console.print(f"[bold cyan]Initiating Sentinel Protocol for:[/bold cyan] {target} [dim](Interval: {telemetry_interval})[/dim]")
    try:
        ansible_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../../QuanuX-Infra/ansible"))
        playbook_path = os.path.join(ansible_dir, "03d-annex-sentinel.yml")
        
        cmd = [
            "ansible-playbook",
            "-i", "dynamic_inventory.py",
            playbook_path,
            "--limit", target,
            "-e", f"telemetry_interval={telemetry_interval}"
        ]
        
        console.print(f"[dim]Executing: {' '.join(cmd)}[/dim]")
        subprocess.run(cmd, cwd=ansible_dir, check=True)
        console.print("[bold green]Success:[/bold green] Sentinel Protocol activated.")
    except subprocess.CalledProcessError:
        console.print("[bold red]Sentinel Protocol Failed.[/bold red] Playbook execution aborted.")
        raise typer.Exit(code=1)

if __name__ == "__main__":
    app()
