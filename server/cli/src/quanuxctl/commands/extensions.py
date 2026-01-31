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

def find_extension_path(ext_name: str) -> Optional[Path]:
    # Check directly first (legacy)
    direct = EXTENSIONS_DIR / ext_name
    if (direct / "extension.yaml").exists():
        return direct
    
    # Check language subdirs
    for lang in ["cpp", "go", "node", "python"]:
        path = EXTENSIONS_DIR / lang / ext_name
        if (path / "extension.yaml").exists():
            return path
    return None

def load_manifest(ext_name: str) -> Optional[dict]:
    path = find_extension_path(ext_name)
    if not path:
        return None
    try:
        with open(path / "extension.yaml", "r") as f:
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

    # Define supported language categories
    languages = ["cpp", "go", "node", "python"]
    
    found_extensions = []
    
    # 1. Check root level (legacy support)
    for item in EXTENSIONS_DIR.iterdir():
        if item.is_dir() and item.name not in languages and (item / "extension.yaml").exists():
            found_extensions.append(item)

    # 2. Check language subdirectories
    for lang in languages:
        lang_dir = EXTENSIONS_DIR / lang
        if lang_dir.exists():
            for item in lang_dir.iterdir():
                if item.is_dir() and (item / "extension.yaml").exists():
                    found_extensions.append(item)

    # Sort by name for consistent output
    found_extensions.sort(key=lambda x: x.name)

    table = Table(title="QuanuX Extensions")
    table.add_column("ID", style="cyan")
    table.add_column("Name", style="green")
    table.add_column("Status", style="bold")
    table.add_column("Runtime", style="magenta")
    table.add_column("Secrets", style="yellow")

    for item in found_extensions:
        # Load manifest directly since we have the path
        try:
            with open(item / "extension.yaml", "r") as f:
                manifest = yaml.safe_load(f)
        except Exception:
            continue
            
        if manifest:
            status, style = get_process_status(item.name)
            secrets_count = len(manifest.get("env", []))
            
            status_cell = f"[{style}]{status}[/{style}]"
            
            # Determine runtime/category from parent dir if not in manifest
            runtime = manifest.get("runtime", item.parent.name if item.parent.name in languages else "unknown")

            table.add_row(
                item.name,
                manifest.get("display_name", item.name),
                status_cell,
                runtime,
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

    ext_path = find_extension_path(name)
    if not ext_path:
        console.print(f"[red]Could not determine path for extension '{name}'.[/red]")
        return

    cmd_rel = manifest.get("command")
    cwd = ext_path.resolve()
    
    # Resolve Command
    if manifest.get("runtime") == "go" and cmd_rel.endswith(".go"):
        final_cmd = ["go", "run", cmd_rel]
    else:
        final_cmd = [str((cwd / cmd_rel).resolve())]

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
    ext_path = find_extension_path(name)
    if not ext_path:
         console.print(f"[red]Path not found for {name}[/red]")
         raise typer.Exit(1)
    cwd = ext_path
    
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
        final_cmd = [str((cwd / cmd_rel).resolve())]

    console.print(f"[blue]Running: {' '.join(final_cmd)}[/blue]")
    try:
        subprocess.run(final_cmd, cwd=cwd, env=env, check=True)
    except KeyboardInterrupt:
        console.print("\n[yellow]Extension stopped.[/yellow]")
    except Exception as e:
        console.print(f"[red]Error running extension: {e}[/red]")

@app.command("install")
def install_extension(name: str, version: str = typer.Option(None, "--version", "-v")):
    """Build/Install the extension (runs build.sh). Use -v to specify version."""
    ext_path = find_extension_path(name)
    if not ext_path:
        console.print(f"[red]Extension '{name}' not found.[/red]")
        raise typer.Exit(1)
        
    build_script = (ext_path / "build.sh").resolve()
    if build_script.exists():
        console.print(f"[green]Building {name}... from {ext_path}[/green]")
        
        # Pass version override if specified
        env = os.environ.copy()
        if version:
            console.print(f"[bold]Target Version: {version}[/bold]")
            env["QUANUX_EXT_VERSION"] = version

        try:
            # Ensure executable
            os.chmod(build_script, 0o755)
            subprocess.run([str(build_script)], cwd=ext_path.resolve(), env=env, check=True)
            console.print(f"[bold green]✓ Build successful[/bold green]")
        except subprocess.CalledProcessError:
            console.print(f"[red]Build failed for {name}[/red]")
            raise typer.Exit(1)
    else:
        console.print(f"[yellow]No build.sh found for {name}. Skipping build step.[/yellow]")
        
    # Python Requirements
    req_file = ext_path / "requirements.txt"
    if req_file.exists():
        console.print(f"[green]Installing dependencies for {name}...[/green]")
        try:
            subprocess.run([sys.executable, "-m", "pip", "install", "-r", str(req_file)], check=True)
        except subprocess.CalledProcessError:
             console.print(f"[red]Failed to install requirements[/red]")

@app.command("upgradeable")
def upgradeable(name: str):
    """Check for available updates (compares installed version vs upstream tags)."""
    manifest = load_manifest(name)
    if not manifest:
        console.print(f"[red]Extension '{name}' not found.[/red]")
        return

    upstream = manifest.get("upstream_repo")
    current_version = manifest.get("version", "unknown")
    
    if not upstream:
        console.print(f"[yellow]No upstream_repo defined for {name}. Cannot check for updates.[/yellow]")
        return
        
    console.print(f"Current Version: [cyan]{current_version}[/cyan]")
    console.print(f"Fetching tags from {upstream}...")
    
    try:
        # Fetch tags via git ls-remote, sort by version
        result = subprocess.run(
            ["git", "ls-remote", "--tags", "--refs", "--sort=-v:refname", upstream],
            capture_output=True, text=True, check=True
        )
        # Parse tags (refs/tags/dest/v1.2.3 -> v1.2.3)
        tags = [line.split("/")[-1] for line in result.stdout.splitlines() if line.strip()]
        
        # Simple heuristic: filter tags that look like versions (vX.Y.Z or X.Y.Z)
        # and take the top 5
        versions = [t for t in tags if "v" in t or "." in t][:5]
        
        console.print("\n[bold]Available Versions (Top 5):[/bold]")
        for v in versions:
            if v == current_version or v == f"v{current_version}":
                 console.print(f"  [green]{v} (Installed)[/green]")
            else:
                 console.print(f"  {v}")
                 
    except subprocess.CalledProcessError as e:
        console.print(f"[red]Failed to fetch tags: {e}[/red]")

@app.command("upgrade")
def upgrade(name: str):
    """Auto-upgrade to the latest version found upstream."""
    manifest = load_manifest(name)
    if not manifest: 
        console.print(f"[red]Extension '{name}' not found.[/red]")
        raise typer.Exit(1)

    upstream = manifest.get("upstream_repo")
    if not upstream:
        console.print(f"[yellow]No upstream_repo defined. Cannot upgrade.[/yellow]")
        raise typer.Exit(1)
        
    try:
        # Fetch status to get latest
        result = subprocess.run(
            ["git", "ls-remote", "--tags", "--refs", "--sort=-v:refname", upstream],
            capture_output=True, text=True, check=True
        )
        lines = result.stdout.splitlines()
        if not lines:
             console.print("[red]No tags found upstream.[/red]")
             return
             
        # Latest is the first one due to sort=-v:refname
        latest_tag = lines[0].split("/")[-1]
        
        console.print(f"[bold green]Upgrading {name} -> {latest_tag}[/bold green]")
        install_extension(name, version=latest_tag)
        
    except subprocess.CalledProcessError as e:
         console.print(f"[red]Upgrade failed: {e}[/red]")

@app.command("uninstall")
def uninstall_extension(name: str, force: bool = typer.Option(False, "--force", "-f")):
    """Clean up build artifacts (removes 'build' directory)."""
    ext_path = find_extension_path(name)
    if not ext_path:
        console.print(f"[red]Extension '{name}' not found.[/red]")
        raise typer.Exit(1)
        
    if not force:
        confirm = typer.confirm(f"Are you sure you want to remove build artifacts for {name}?")
        if not confirm:
            return

    build_dir = ext_path / "build"
    if build_dir.exists() and build_dir.is_dir():
        import shutil
        shutil.rmtree(build_dir)
        console.print(f"[green]Removed build directory for {name}.[/green]")
    else:
        console.print(f"[yellow]No build directory found for {name}.[/yellow]")

@app.command("remove")
def remove_extension(name: str, force: bool = typer.Option(False, "--force", "-f")):
    """Alias for uninstall."""
    uninstall_extension(name, force)

