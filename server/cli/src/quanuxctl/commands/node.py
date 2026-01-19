import typer
import subprocess
from rich.console import Console
from rich.prompt import Prompt

app = typer.Typer(no_args_is_help=True)
console = Console()

@app.command()
def deploy(
    target: str = typer.Argument(..., help="SSH target (e.g. user@192.168.1.50)"),
    hub: str = typer.Option(..., help="NATS Hub URL (e.g. nats://hub.quanux.io:4222)"),
    token: str = typer.Option(..., help="Registration Token"),
    dry_run: bool = typer.Option(False, help="Print command without executing")
):
    """
    Deploy the QuanuX Execution Node to a remote server via SSH.
    This pushes the bootstrap script and runs it.
    """
    console.print(f"[bold blue]Deploying Execution Node to {target}...[/bold blue]")

    # Ideally we host the script at a URL, but for now we can cat it over SSH
    # or just run the curl command if we assume the remote has internet.
    # Let's assume the remote can `curl` our install script.
    
    # 1. Construct the remote command
    # We use 'bash -s' to pipe args to the script
    install_url = "https://quanux.io/install-node" # Placeholder URL
    remote_cmd = f"curl -sL {install_url} | sudo bash -s -- --hub {hub} --token {token}"
    
    ssh_cmd = ["ssh", "-o", "StrictHostKeyChecking=accept-new", target, remote_cmd]

    if dry_run:
        console.print("[dim]Dry run:[/dim]")
        console.print(f"  {' '.join(ssh_cmd)}")
        return

    try:
        console.print(f"Running: {' '.join(ssh_cmd)}")
        # We use check=True to raise error on non-zero exit
        subprocess.run(ssh_cmd, check=True)
        console.print(f"[bold green]✅ Deployment to {target} successful![/bold green]")
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]❌ Deployment failed with exit code {e.returncode}[/bold red]")
        raise typer.Exit(1)
