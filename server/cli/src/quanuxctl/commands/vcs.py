import typer
from rich.console import Console
import subprocess
from pathlib import Path

app = typer.Typer(help="Version Control System Connectors (Git integration).")
console = Console()

def run_git_cmd(args: list[str], cwd: Path = None):
    try:
        subprocess.run(["git"] + args, cwd=cwd, check=True)
    except subprocess.CalledProcessError as e:
        console.print(f"[red]Git command failed: {e}[/red]")
    except FileNotFoundError:
        console.print("[red]Error: 'git' executable not found. Please install Git.[/red]")

@app.command()
def clone(
    url: str = typer.Argument(..., help="Repository URL to clone"),
    target: str = typer.Option(None, help="Target directory name")
):
    """
    Clone a remote repository.
    """
    cmd = ["clone", url]
    if target:
        cmd.append(target)
    
    console.print(f"[bold blue]Cloning {url}...[/bold blue]")
    run_git_cmd(cmd)

@app.command()
def status():
    """
    Show status of the current repository.
    """
    run_git_cmd(["status"])

@app.command()
def sync():
    """
    Pull latest changes from the default remote (origin).
    """
    console.print("[bold blue]Syncing (pulling) changes...[/bold blue]")
    run_git_cmd(["pull"])

@app.command()
def commit(
    message: str = typer.Option(..., "-m", "--message", help="Commit message"),
    all: bool = typer.Option(False, "-a", "--all", help="Stage all modified files")
):
    """
    Commit changes to the local repository.
    """
    if all:
        run_git_cmd(["add", "-u"])
    
    run_git_cmd(["commit", "-m", message])

@app.command()
def push(
    remote: str = typer.Option("origin", help="Remote name"),
    branch: str = typer.Option(None, help="Branch name (default: current)")
):
    """
    Push changes to remote.
    """
    cmd = ["push", remote]
    if branch:
        cmd.append(branch)
    
    console.print(f"[bold blue]Pushing to {remote}...[/bold blue]")
    run_git_cmd(cmd)

@app.command()
def connect(
    url: str = typer.Argument(..., help="Remote URL"),
    name: str = typer.Option("origin", help="Remote name")
):
    """
    Connect current directory to a remote VCS.
    """
    run_git_cmd(["remote", "add", name, url])
    console.print(f"[green]Connected remote '{name}' to {url}[/green]")
