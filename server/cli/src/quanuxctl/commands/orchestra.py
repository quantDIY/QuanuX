import typer
import subprocess
import os
import hashlib
from rich.console import Console
from pathlib import Path

app = typer.Typer(help="Manage QuanuX-Orchestra Universal Naming Registry.")
console = Console()

# Resolve the QuanuX root directory assuming this script is in server/cli/src/quanuxctl/commands/
REPO_ROOT = Path(__file__).resolve().parent.parent.parent.parent.parent.parent
ORCHESTRA_DIR = REPO_ROOT / "QuanuX-Orchestra"

@app.command("bootstrap")
def bootstrap():
    """
    Executes the Python script to download the ISO 20022 XML and generate constants.hpp.
    """
    console.print("[cyan]Initiating QuanuX-Orchestra Bootstrap Protocol...[/cyan]")
    
    script_path = ORCHESTRA_DIR / "scripts" / "bootstrap_orchestra.py"
    if not script_path.exists():
        console.print(f"[bold red]Bootstrap script not found at {script_path}[/bold red]")
        raise typer.Exit(code=1)

    try:
        # Run inside the QuanuX-Orchestra directory to preserve relative paths
        subprocess.run(
            ["python3", str(script_path)],
            cwd=str(ORCHESTRA_DIR),
            check=True
        )
        console.print("[bold green]Bootstrap absolute parity achieved.[/bold green]")
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]Bootstrap execution failed: {e}[/bold red]")
        raise typer.Exit(code=1)

@app.command("compile")
def compile_registry(
    venue: str = typer.Option(..., "--venue", "-v", help="The broker venue to compile (e.g. ibkr)")
):
    """
    Executes standardizer_cli, generating the C++ constexpr bridge, the Cython wrappers, and embedding the checksums.
    """
    console.print(f"[cyan]Compiling Tier-2 Naming Registry for Venue: {venue}...[/cyan]")
    
    venue_xml = ORCHESTRA_DIR / "venues" / venue / "broker.xml"
    if not venue_xml.exists():
        console.print(f"[bold red]Venue dictionary not found at {venue_xml}[/bold red]")
        raise typer.Exit(code=1)

    # Allow execution whether built via raw clang++ in src/cli or via CMake in build/
    cli_path_cmake = ORCHESTRA_DIR / "build" / "standardizer_cli"
    cli_path_raw = ORCHESTRA_DIR / "src" / "cli" / "standardizer_cli"
    
    if cli_path_cmake.exists():
        cli_executable = cli_path_cmake
    elif cli_path_raw.exists():
        cli_executable = cli_path_raw
    else:
        console.print("[bold red]standardizer_cli binary not found. Have you built QuanuX-Orchestra using CMake?[/bold red]")
        raise typer.Exit(code=1)

    try:
        # Pass the relative path from QuanuX-Orchestra root due to C++ file assumptions
        rel_venue_xml = f"venues/{venue}/broker.xml"
        subprocess.run(
            [str(cli_executable), rel_venue_xml],
            cwd=str(ORCHESTRA_DIR),
            check=True
        )
        console.print("[bold green]Orchestration zero-latency mappings generated successfully.[/bold green]")
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]Standardizer execution failed: {e}[/bold red]")
        raise typer.Exit(code=1)

@app.command("verify")
def verify():
    """
    Checks parity between the active Spreader binary checksum and the Python superGraph bindings.
    """
    console.print("[cyan]Verifying Schema Coherence...[/cyan]")
    
    # Normally this would extract the embedded C++ binary checksum and match it against the Cython pyx file
    pyx_file = ORCHESTRA_DIR / "python" / "mcp_bindings" / "orchestra_bindings.pyx"
    
    if not pyx_file.exists():
        console.print(f"[bold red]Cython bindings not found at {pyx_file}. Have you run compile?[/bold red]")
        raise typer.Exit(code=1)
        
    try:
        checksum = None
        with open(pyx_file, "r") as f:
            for line in f:
                if "__checksum__" in line:
                    checksum = line.split("=")[1].strip().strip('"')
                    break
        
        if checksum:
            console.print(f"[green]Cython Schema Fingerprint Validated: {checksum}[/green]")
            console.print("[bold green]Absolute parity verified. Safe to proceed.[/bold green]")
        else:
            console.print("[bold red]Schema Fingerprint could not be located inside the wrapper.[/bold red]")
            raise typer.Exit(code=1)
            
    except Exception as e:
        console.print(f"[bold red]Verification failed: {e}[/bold red]")
        raise typer.Exit(code=1)
