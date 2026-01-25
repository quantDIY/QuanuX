
import typer
from rich.console import Console
from rich.table import Table
import subprocess
import os
from pathlib import Path

app = typer.Typer(help="Manage Indicator Registry (Community Extensions).")
console = Console()

CONTRIB_DIR = Path("server/indicators/include/contrib")

@app.command()
def list():
    """
    List installed community indicators.
    """
    if not CONTRIB_DIR.exists():
        console.print("[yellow]No community indicators installed.[/yellow]")
        return
        
    table = Table(title="Community Indicators")
    table.add_column("Name", style="cyan")
    table.add_column("Path", style="dim")
    
    for item in CONTRIB_DIR.iterdir():
        if item.is_dir():
            table.add_row(item.name, str(item))
            
    console.print(table)

@app.command()
def install(
    url: str = typer.Argument(..., help="Git URL of the indicator repository"),
    name: str = typer.Option(None, help="Local name for the indicator package")
):
    """
    Install a community indicator from Git.
    """
    if not name:
        # Infer name from URL
        name = url.split("/")[-1].replace(".git", "")
        
    target_path = CONTRIB_DIR / name
    
    if target_path.exists():
        console.print(f"[red]Error: Indicator '{name}' already exists.[/red]")
        raise typer.Exit(1)
        
    console.print(f"[blue]Cloning {url} into {target_path}...[/blue]")
    try:
        # Create contrib dir if not exists
        CONTRIB_DIR.mkdir(parents=True, exist_ok=True)
        
        subprocess.run(["git", "clone", url, str(target_path)], check=True)
        console.print(f"[green]Successfully installed {name}.[/green]")
        console.print(f"[dim]Include it via: #include \"contrib/{name}/<header>.hpp\"[/dim]")
    except subprocess.CalledProcessError as e:
        console.print(f"[red]Failed to clone repo: {e}[/red]")
        # Cleanup partial
        if target_path.exists():
            shutil.rmtree(target_path)

@app.command()
def remove(
    name: str = typer.Argument(..., help="Name of the indicator package to remove")
):
    """
    Remove a community indicator.
    """
    target_path = CONTRIB_DIR / name
    
    if not target_path.exists():
        console.print(f"[red]Error: Indicator '{name}' not found.[/red]")
        raise typer.Exit(1)
        
    confirm = typer.confirm(f"Are you sure you want to remove {name}?")
    if not confirm:
        raise typer.Abort()
        
    import shutil
    shutil.rmtree(target_path)
    console.print(f"[green]Removed {name}.[/green]")
