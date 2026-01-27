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

# --- Advanced Connectors ---

@app.command()
def setup(
    provider: str = typer.Argument(..., help="Provider name (github, gitlab)"),
    token: str = typer.Option(None, prompt=True, hide_input=True, help="Personal Access Token")
):
    """
    Configure credentials for a VCS provider (GitHub, GitLab).
    """
    from ...lib.vcs_providers import get_provider
    
    prov = get_provider(provider)
    if not prov:
        console.print(f"[red]Unsupported provider: {provider}[/red]")
        return

    if token:
        prov.set_token(token)

@app.command()
def publish(
    provider: str = typer.Argument(..., help="Provider name (github, gitlab)"),
    name: str = typer.Option(None, help="Repository name (defaults to current folder)"),
    private: bool = typer.Option(True, help="Create as private repository")
):
    """
    Create a remote repository on the provider and push the current project to it.
    """
    from ...lib.vcs_providers import get_provider
    import os
    
    prov = get_provider(provider)
    if not prov:
        console.print(f"[red]Unsupported provider: {provider}[/red]")
        return

    if not name:
        name = os.path.basename(os.getcwd())
    
    # 1. Create Remote
    console.print(f"[bold blue]Creating {provider} repository '{name}'...[/bold blue]")
    repo_url = prov.create_repo(name, private)
    
    if repo_url:
        # 2. Local Git Init (idempotent)
        if not os.path.exists(".git"):
            run_git_cmd(["init"])
            # Initial add just in case
            run_git_cmd(["add", "."])
            run_git_cmd(["commit", "-m", "Initial commit via QuanuX"])

        # 3. Add Remote
        # Check if origin exists
        try:
             # This will fail if remote exists, handled below
             run_git_cmd(["remote", "add", "origin", repo_url])
        except Exception:
             console.print("[yellow]Remote 'origin' already exists. Setting URL...[/yellow]")
             run_git_cmd(["remote", "set-url", "origin", repo_url])
             
        # 4. Push
        console.print("[bold blue]Pushing to new remote...[/bold blue]")
        # Provide upstream set
        run_git_cmd(["push", "-u", "origin", "main"]) # or master, depending on git config, but main is modern standard

