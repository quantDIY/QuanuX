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
        ansible_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../ansible"))
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

if __name__ == "__main__":
    app()
