import typer
import os
import platform
from pathlib import Path
from rich.console import Console
from rich.table import Table

app = typer.Typer()
console = Console()

def get_mount_roots():
    system = platform.system()
    roots = []
    if system == "Darwin":
        roots.append(Path("/Volumes"))
    elif system == "Linux":
        roots.append(Path("/mnt"))
        roots.append(Path("/media"))
    return roots

@app.command("scan")
def scan_storage():
    """Scan for connected storage arrays and volumes."""
    roots = get_mount_roots()
    
    table = Table(title="Connected Storage Volumes")
    table.add_column("Name", style="cyan")
    table.add_column("Path", style="green")
    table.add_column("Size", style="dim")
    table.add_column("DuckDB Files", style="yellow")

    found_any = False

    for root in roots:
        if not root.exists():
            continue
            
        try:
            # List immediate subdirectories (mount points)
            for item in root.iterdir():
                if item.is_dir() and not item.name.startswith('.'):
                    found_any = True
                    
                    # Heuristic check for DuckDB files
                    duck_files = 0
                    try:
                        # Quick shallow scan for database files
                        # We don't want to define a heavy specialized check yet, just count *.duckdb or *.db
                        # Limit recursion or count
                        duck_files = len(list(item.glob("*.duckdb"))) + len(list(item.glob("*.db")))
                    except Exception:
                        pass
                    
                    # Size calculation (expensive on network mounts, maybe skip or use os.statvfs)
                    size_str = "Unknown"
                    try:
                        # statvfs for total size of file system?
                        stat = os.statvfs(item)
                        total_gb = (stat.f_blocks * stat.f_frsize) / (1024**3)
                        size_str = f"{total_gb:.1f} GB"
                    except Exception:
                        pass

                    table.add_row(item.name, str(item), size_str, str(duck_files) if duck_files > 0 else "")
        except Exception as e:
            console.print(f"[red]Error scanning {root}: {e}[/red]")

    if found_any:
        console.print(table)
    else:
        console.print("[yellow]No external volumes found in standard locations.[/yellow]")
        console.print(f"Checked: {', '.join([str(r) for r in roots])}")

        console.print(f"Checked: {', '.join([str(r) for r in roots])}")

# DuckDB Sub-app
duckdb_app = typer.Typer(help="Manage DuckDB cartridges.")
app.add_typer(duckdb_app, name="duckdb")

@duckdb_app.command("init")
def init_db(path: str):
    """Initialize a new empty DuckDB file."""
    p = Path(path)
    if p.exists():
        console.print(f"[yellow]File {path} already exists.[/yellow]")
        return
    
    try:
        # Import here to avoid dependency if not used
        import duckdb
        conn = duckdb.connect(str(p))
        conn.close()
        console.print(f"[green]Initialized empty DuckDB at {path}[/green]")
    except ImportError:
        console.print("[red]duckdb python package not installed. Run 'pip install duckdb'[/red]")
        # Or use quanux_duckdb if available

@duckdb_app.command("attach")
def attach_db(path: str, alias: str, server_url: str = typer.Option("http://localhost:8000", help="QuanuX Server URL")):
    """Attach a DB file to the running QuanuX Server."""
    import requests
    try:
        res = requests.post(f"{server_url}/api/storage/attach", json={"path": path, "alias": alias})
        if res.ok:
            console.print(f"[green]Successfully attached {alias}[/green]")
        else:
            console.print(f"[red]Failed: {res.text}[/red]")
    except Exception as e:
        console.print(f"[red]Connection error: {e}[/red]")

@duckdb_app.command("detach")
def detach_db(alias: str, server_url: str = typer.Option("http://localhost:8000", help="QuanuX Server URL")):
    """Detach a DB file from the QuanuX Server."""
    import requests
    try:
        res = requests.post(f"{server_url}/api/storage/detach", json={"alias": alias})
        if res.ok:
            console.print(f"[green]Successfully detached {alias}[/green]")
        else:
            console.print(f"[red]Failed: {res.text}[/red]")
    except Exception as e:
        console.print(f"[red]Connection error: {e}[/red]")
