import typer
import yaml
import os
import subprocess
import sys
from pathlib import Path
from rich.console import Console
from rich.table import Table
from typing import Optional
from server.security.secrets import SecretsBroker, KeyringBackend

app = typer.Typer()
console = Console()

EXTENSIONS_DIR = Path("extensions")

def get_broker():
    return SecretsBroker(primary=KeyringBackend(), fallback=[])

def load_manifest(ext_name: str) -> Optional[dict]:
    manifest_path = EXTENSIONS_DIR / ext_name / "extension.yaml"
    if not manifest_path.exists():
        return None
    try:
        with open(manifest_path, "r") as f:
            return yaml.safe_load(f)
    except Exception as e:
        console.print(f"[red]Error loading manifest for {ext_name}: {e}[/red]")
        return None

@app.command("list")
def list_extensions():
    """List all installed extensions and their status."""
    if not EXTENSIONS_DIR.exists():
        console.print("[yellow]No extensions directory found.[/yellow]")
        return

    table = Table(title="QuanuX Extensions")
    table.add_column("ID", style="cyan")
    table.add_column("Name", style="green")
    table.add_column("Status", style="bold")
    table.add_column("Runtime", style="magenta")
    table.add_column("Secrets", style="yellow")

    for item in EXTENSIONS_DIR.iterdir():
        if item.is_dir() and (item / "extension.yaml").exists():
            manifest = load_manifest(item.name)
            if manifest:
                status, style = get_process_status(item.name)
                secrets_count = len(manifest.get("env", []))
                
                status_cell = f"[{style}]{status}[/{style}]"
                
                table.add_row(
                    item.name,
                    manifest.get("display_name", item.name),
                    status_cell,
                    manifest.get("runtime", "unknown"),
                    str(secrets_count)
                )

    console.print(table)

RUN_DIR = Path.home() / ".quanux" / "run"
LOG_DIR = Path.home() / ".quanux" / "logs"

def get_pid_file(name: str) -> Path:
    RUN_DIR.mkdir(parents=True, exist_ok=True)
    return RUN_DIR / f"{name}.pid"

def get_log_file(name: str) -> Path:
    LOG_DIR.mkdir(parents=True, exist_ok=True)
    return LOG_DIR / f"{name}.log"

def rotate_logs(name: str):
    """Simple log rotation: if > 5MB, keep one backup."""
    log_file = get_log_file(name)
    if not log_file.exists():
        return
    
    try:
        if log_file.stat().st_size > 5 * 1024 * 1024: # 5MB
            backup = log_file.with_suffix(".log.old")
            log_file.rename(backup)
            console.print(f"[yellow]Rotated log file for {name} (>5MB).[/yellow]")
    except Exception as e:
        console.print(f"[red]Error rotating logs: {e}[/red]")

def get_process_status(name: str) -> tuple[str, str]:
    """Returns (status_code, style). Status: RUNNING, STOPPED, DEAD, CORRUPT."""
    pid_file = get_pid_file(name)
    if not pid_file.exists():
        return "STOPPED", "dim"
    
    try:
        with open(pid_file, "r") as f:
            content = f.read().strip()
            if not content:
                 return "CORRUPT", "red"
            pid = int(content)
        os.kill(pid, 0)
        return f"RUNNING ({pid})", "bold green"
    except OSError:
        return "DEAD (Stale PID)", "red"
    except ValueError:
        return "CORRUPT", "red"

@app.command("start")
def start_extension(name: str):
    """Start an extension in the background."""
    manifest = load_manifest(name)
    if not manifest:
        console.print(f"[red]Extension '{name}' not found.[/red]")
        raise typer.Exit(1)
        
    pid_file = get_pid_file(name)
    # ... check pid_file ...
    if pid_file.exists():
         console.print(f"[yellow]Extension '{name}' might already be running (PID file exists). Run 'stop' first or check status.[/yellow]")
         return

    # Rotate logs before starting
    rotate_logs(name)

    cmd_rel = manifest.get("command")
    cwd = EXTENSIONS_DIR / name
    
    # Resolve Command
    if manifest.get("runtime") == "go" and cmd_rel.endswith(".go"):
        final_cmd = ["go", "run", cmd_rel]
    else:
        final_cmd = [str(cwd / cmd_rel)]

    # Inject Secrets
    env = os.environ.copy()
    broker = get_broker()
    console.print(f"[bold]Injecting Secrets for {name}...[/bold]")
    for secret_key in manifest.get("env", []):
        val = broker.get(secret_key)
        if val:
            env[secret_key] = val

    log_file = get_log_file(name)
    console.print(f"[green]Starting {name} in background...[/green]")
    console.print(f"Logs: {log_file}")

    with open(log_file, "a") as log_out:
        proc = subprocess.Popen(
            final_cmd,
            cwd=cwd,
            env=env,
            stdout=log_out,
            stderr=subprocess.STDOUT,
            start_new_session=True # Detach
        )
    
    with open(pid_file, "w") as f:
        f.write(str(proc.pid))
    
    console.print(f"[green]Started! PID: {proc.pid}[/green]")

@app.command("stop")
def stop_extension(name: str):
    """Stop a running extension."""
    pid_file = get_pid_file(name)
    if not pid_file.exists():
        console.print(f"[yellow]Extension '{name}' is not running.[/yellow]")
        return
    
    try:
        with open(pid_file, "r") as f:
            pid = int(f.read().strip())
        
        os.kill(pid, 15) # SIGTERM
        console.print(f"[green]Stopped {name} (PID {pid}).[/green]")
        pid_file.unlink()
    except ProcessLookupError:
        console.print(f"[yellow]Process {pid} not found. Cleaning up stale PID file.[/yellow]")
        pid_file.unlink()
    except Exception as e:
        console.print(f"[red]Error stopping {name}: {e}[/red]")

@app.command("status")
def extension_status(name: str):
    """Check if an extension is running."""
    pid_file = get_pid_file(name)
    if not pid_file.exists():
        console.print(f"[red]{name} is STOPPED[/red]")
        return
    
    try:
        with open(pid_file, "r") as f:
            pid = int(f.read().strip())
        os.kill(pid, 0) # Check if process exists
        console.print(f"[green]{name} is RUNNING (PID {pid})[/green]")
        console.print(f"Log: {get_log_file(name)}")
    except OSError:
        console.print(f"[red]{name} is DEAD (Stale PID file found)[/red]")

@app.command("clean")
def clean_extension(name: str = typer.Argument(None, help="Extension name or 'all'"), 
                   all: bool = typer.Option(False, "--all", help="Clean all extensions")):
    """Remove logs and runtime files."""
    
    def _clean_one(n: str):
        # Stop if running
        pid_file = get_pid_file(n)
        if pid_file.exists():
            try:
                # Try to check if running, if so, warn or stop?
                # For clean, we usually imply stop.
                with open(pid_file, "r") as f:
                    pid = int(f.read().strip())
                os.kill(pid, 0)
                console.print(f"[red]Extension '{n}' is running (PID {pid}). Stop it first.[/red]")
                return
            except OSError:
                # Process dead, safe to clean
                pass
            pid_file.unlink()

        # Remove logs
        log_file = get_log_file(n)
        old_log = log_file.with_suffix(".log.old")
        
        cleaned = False
        if log_file.exists():
            log_file.unlink()
            cleaned = True
        if old_log.exists():
            old_log.unlink()
            cleaned = True
            
        if cleaned:
            console.print(f"[green]Cleaned logs for {n}.[/green]")
        else:
            console.print(f"[dim]No logs found for {n}.[/dim]")

    if all or name == "all":
        if not EXTENSIONS_DIR.exists():
            return
        console.print("[bold]Cleaning all extensions...[/bold]")
        for item in EXTENSIONS_DIR.iterdir():
            if item.is_dir() and (item / "extension.yaml").exists():
                _clean_one(item.name)
    elif name:
        _clean_one(name)
    else:
        console.print("[yellow]Please specify extension name or --all[/yellow]")

@app.command("run")
def run_extension(name: str):
    """Run an extension in the foreground (injecting secrets)."""
    # ... existing implementation ...
    manifest = load_manifest(name)
    if not manifest:
        console.print(f"[red]Extension '{name}' not found.[/red]")
        raise typer.Exit(1)

    cmd_rel = manifest.get("command")
    cwd = EXTENSIONS_DIR / name
    
    env = os.environ.copy()
    broker = get_broker()
    
    console.print(f"[bold]Injecting Secrets for {name}...[/bold]")
    for secret_key in manifest.get("env", []):
        val = broker.get(secret_key)
        if val:
            env[secret_key] = val
        else:
             console.print(f"  [red]✗[/red] {secret_key}")

    if manifest.get("runtime") == "go" and cmd_rel.endswith(".go"):
        final_cmd = ["go", "run", cmd_rel]
    else:
        final_cmd = [str(cwd / cmd_rel)]

    console.print(f"[blue]Running: {' '.join(final_cmd)}[/blue]")
    try:
        subprocess.run(final_cmd, cwd=cwd, env=env, check=True)
    except KeyboardInterrupt:
        console.print("\n[yellow]Extension stopped.[/yellow]")
    except Exception as e:
        console.print(f"[red]Error running extension: {e}[/red]")
