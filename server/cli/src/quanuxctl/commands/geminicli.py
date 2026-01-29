import typer
import subprocess
import sys
from pathlib import Path
from rich.console import Console

app = typer.Typer()
console = Console()

def get_project_root() -> Path:
    """Find the project root."""
    # Assuming this is run from within the repo, finding where 'quanuxctl' or 'server' is
    # Using the current file path
    current_file = Path(__file__).resolve()
    # Path: server/cli/src/quanuxctl/commands/geminicli.py
    # Root: ../../../../../../
    return current_file.parents[5]

@app.command()
def install():
    """
    Install the QuanuX Gemini CLI Extension locally.
    """
    root = get_project_root()
    extension_path = root / "extensions" / "python" / "gemini-cli-extension"
    
    if not extension_path.exists():
        console.print(f"[bold red]Error:[/bold red] Extension path not found: {extension_path}")
        raise typer.Exit(1)
        
    console.print(f"[bold blue]Installing extension from:[/bold blue] {extension_path}")
    
    try:
        # Check if gemini is installed
        subprocess.run(["gemini", "--version"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        
        # Install
        subprocess.run(["gemini", "extension", "install", str(extension_path)], check=True)
        console.print("[bold green]Successfully installed QuanuX Gemini Extension![/bold green]")
        
    except FileNotFoundError:
        console.print("[bold red]Error:[/bold red] 'gemini' command not found. Please install Gemini CLI first.")
        raise typer.Exit(1)
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]Installation failed:[/bold red] {e}")
        raise typer.Exit(1)

@app.command()
def remove():
    """
    Remove the QuanuX Gemini CLI Extension.
    """
    try:
         # Check if gemini is installed
        subprocess.run(["gemini", "--version"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        
        # Uninstall using the ID defined in gemini-extension.json
        subprocess.run(["gemini", "extension", "uninstall", "quanux-mcp"], check=True)
        console.print("[bold green]Successfully removed QuanuX Gemini Extension.[/bold green]")
        
    except FileNotFoundError:
        console.print("[bold red]Error:[/bold red] 'gemini' command not found.")
        raise typer.Exit(1)
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]Removal failed:[/bold red] {e}")
        raise typer.Exit(1)
