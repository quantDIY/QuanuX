import typer
from rich.console import Console
import subprocess
import os

app = typer.Typer(help="QuanuX Control Plane: Nest Operations")
console = Console()

@app.command("drop")
def drop(
    target: str = typer.Argument(..., help="Ansible inventory target (e.g., edge_nodes)"),
    engine: str = typer.Option("spreader", help="Engine type to compile and bind")
):
    """
    Deploys the C++ execution logic into a pre-conditioned OS Habitat.
    Autonomously compiles the binary natively and binds it to the SystemD matrix.
    """
    console.print(f"[bold yellow]Initiating Nest Drop:[/bold yellow] {engine} -> {target}")
    try:
        ansible_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../ansible"))
        playbook_path = os.path.join(ansible_dir, "03b-nest-drop.yml")
        
        cmd = [
            "ansible-playbook",
            "-i", "dynamic_inventory.py",
            playbook_path,
            "--limit", target,
            "-e", f"quanux_engine_type={engine}"
        ]
        
        console.print(f"[dim]Executing: {' '.join(cmd)}[/dim]")
        subprocess.run(cmd, cwd=ansible_dir, check=True)
        console.print("[bold green]Success:[/bold green] Nest dropped. The Engine is howling.")
    except subprocess.CalledProcessError as e:
        console.print("[bold red]Nest Drop Failed.[/bold red] Execution binary rejected by Habitat.")
        raise typer.Exit(code=1)

if __name__ == "__main__":
    app()
