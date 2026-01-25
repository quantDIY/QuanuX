
import typer
from rich.console import Console
import shutil
import os
from pathlib import Path

app = typer.Typer(help="Manage QuanuX Core Modules (Lifecycle).")
console = Console()

INDICATORS_DIR = Path("server/indicators")
CMAKE_FILE = Path("server/indicators/CMakeLists.txt")

@app.command()
def remove(
    module: str = typer.Argument(..., help="Module name (e.g. 'indicators')")
):
    """
    Uninstall a core module (Clean Removal).
    """
    if module != "indicators":
        console.print(f"[red]Error: Module '{module}' not recognized or does not support removal.[/red]")
        raise typer.Exit(1)

    if not INDICATORS_DIR.exists():
        console.print("[yellow]Module 'indicators' is not installed.[/yellow]")
        return

    confirm = typer.confirm(f"Are you sure you want to potentially DESTRUCTIVE delete {INDICATORS_DIR}? This cannot be undone.")
    if not confirm:
        raise typer.Abort()

    try:
        shutil.rmtree(INDICATORS_DIR)
        console.print(f"[green]Successfully removed {INDICATORS_DIR}[/green]")
        # Ideally we would also edit the parent CMakeLists.txt to remove add_subdirectory
        # But for this prototype we assume the user might manually need to do that or we can automate it
        # For this specific task, we'll just log it.
        console.print("[bold yellow]Please manually remove 'add_subdirectory(pybind)' and relevant lines from CMakeLists.txt if needed.[/bold yellow]")
    except Exception as e:
        console.print(f"[red]Failed to remove module: {e}[/red]")

@app.command()
def install(
    module: str = typer.Argument(..., help="Module name (e.g. 'indicators')")
):
    """
    Re-install a core module (Restore form).
    """
    if module != "indicators":
        console.print(f"[red]Error: Module '{module}' not recognized.[/red]")
        raise typer.Exit(1)
        
    if INDICATORS_DIR.exists():
        console.print("[yellow]Module 'indicators' is already installed.[/yellow]")
        return
        
    console.print("[blue]Restoring indicators module scaffolding...[/blue]")
    # In a real system, this might pull from a git submodule or a scaffold template
    # For now, we will just create the dir to prove the concept
    INDICATORS_DIR.mkdir(parents=True, exist_ok=True)
    console.print(f"[green]Created {INDICATORS_DIR}. Please run CMake configuration to restore.[/green]")

@app.command()
def check(
    module: str = typer.Argument(..., help="Module name (e.g. 'indicators')")
):
    """
    Verify module integrity.
    """
    if module == "indicators":
        if not INDICATORS_DIR.exists():
             console.print("[red]FAIL: Module directory missing.[/red]")
             raise typer.Exit(1)
        # Check for key artifacts
        if (INDICATORS_DIR / "include/quanux/indicators/view.hpp").exists():
             console.print("[green]PASS: Core headers found.[/green]")
        else:
             console.print("[red]FAIL: Core headers missing.[/red]")
    else:
        console.print(f"[yellow]Unknown module {module}[/yellow]")
