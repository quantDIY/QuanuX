import typer
import subprocess
import os
import psutil
from rich.console import Console
from pathlib import Path

app = typer.Typer(help="Manage QuanuX Crucible Backtesting Engine.")
console = Console()

CRUCIBLE_PID_FILE = Path("/tmp/quanux_crucible.pid")

def get_crucible_pid():
    if CRUCIBLE_PID_FILE.exists():
        try:
            return int(CRUCIBLE_PID_FILE.read_text().strip())
        except ValueError:
            pass
    return None

@app.command("start")
def start(
    strategy: str = typer.Argument(..., help="The name of the strategy to backtest."),
    version: str = typer.Option("1.0.0", help="The version of the strategy.")
):
    """
    Start an isolated QuanuX Crucible backtest run.
    """
    pid = get_crucible_pid()
    if pid and psutil.pid_exists(pid):
        console.print(f"[bold red]Crucible is already running on PID {pid}. Please stop it first.[/bold red]")
        raise typer.Exit(code=1)

    harness_path = Path(f"server/backtests/{strategy}_v{version}/crucible_harness_python.py")
    if not harness_path.exists():
        console.print(f"[bold red]Harness not found at {harness_path}. Has it been generated via Foundry?[/bold red]")
        raise typer.Exit(code=1)
    
    console.print(f"[cyan]Igniting QuanuX Crucible Engine for {strategy} v{version}...[/cyan]")
    
    # Isolate PID
    process = subprocess.Popen(
        ["python3", str(harness_path)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        preexec_fn=os.setsid
    )
    
    CRUCIBLE_PID_FILE.write_text(str(process.pid))
    console.print(f"[bold green]Crucible backtest started successfully! (PID {process.pid})[/bold green]")
    console.print(f"Watch live telemetry via NATS: sys.crucible.report.{strategy}")

@app.command("stop")
def stop():
    """
    Stop the currently running Crucible backtest.
    """
    pid = get_crucible_pid()
    if not pid or not psutil.pid_exists(pid):
        console.print("[yellow]Crucible is not currently running.[/yellow]")
        if CRUCIBLE_PID_FILE.exists():
            CRUCIBLE_PID_FILE.unlink()
        return

    console.print(f"[yellow]Stopping Crucible Engine (PID {pid})...[/yellow]")
    try:
        process = psutil.Process(pid)
        process.terminate()
        process.wait(timeout=5)
    except psutil.NoSuchProcess:
        pass
    except psutil.TimeoutExpired:
        console.print("[red]Process did not terminate gracefully. Killing...[/red]")
        process.kill()
    
    CRUCIBLE_PID_FILE.unlink(missing_ok=True)
    console.print("[bold green]Crucible Engine stopped.[/bold green]")

@app.command("status")
def status():
    """
    Check the status of the Crucible Engine.
    """
    pid = get_crucible_pid()
    if pid and psutil.pid_exists(pid):
        console.print(f"[bold green]Crucible Backtester is RUNNING (PID {pid})[/bold green]")
        process = psutil.Process(pid)
        cpu = process.cpu_percent(interval=0.1)
        mem = process.memory_info().rss / (1024 * 1024)
        console.print(f"CPU Matcher Load: {cpu:.1f}% | Aligned RAM Pool: {mem:.1f} MB")
    else:
        console.print("[yellow]Crucible Backtester is STOPPED.[/yellow]")
        if CRUCIBLE_PID_FILE.exists():
            CRUCIBLE_PID_FILE.unlink()

@app.command("report")
def report(
    strategy: str = typer.Argument(..., help="The name of the strategy to pull metrics for."),
    version: str = typer.Option("1.0.0", help="The version of the strategy.")
):
    """
    Retrieve instantaneous cache-aligned Backtest Metrics directly from the C++ Engine via Cython.
    """
    db_path = Path(f"server/backtests/{strategy}_v{version}/crucible.duckdb")
    
    if not db_path.exists():
        console.print(f"[yellow]No backtest data found for {strategy} v{version}.[/yellow]")
        console.print(f"Run 'quanuxctl crucible start {strategy} --version {version}' to generate metrics.")
        raise typer.Exit(code=0)

    # We dynamically load the Cython extension to avoid slowing down other CLI commands
    try:
        import sys
        # Map back to QuanuX-Backtesting-Engine/python
        repo_root = Path(__file__).resolve().parent.parent.parent.parent.parent
        engine_path = repo_root / "QuanuX-Backtesting-Engine" / "python"
        
        if str(engine_path) not in sys.path:
            sys.path.insert(0, str(engine_path))
            
        import quanux_backtest.cython.quanux_crucible as qc
    except ImportError as e:
        console.print(f"[bold red]Failed to load Cython Engine Bindings: {e}[/bold red]")
        raise typer.Exit(code=1)
        
    try:
        # Instantiate the C++ DuckDB Feeder through Cython
        feeder = qc.PyDuckDBFeeder(str(db_path))
        
        # Pull the JSON string straight from the DuckDB C API
        metrics_json = feeder.get_metrics_json(strategy)
        
        console.print(f"[bold cyan]Crucible Telemetry for {strategy} v{version}:[/bold cyan]")
        console.print(metrics_json)
        
    except Exception as e:
        console.print(f"[bold red]DuckDB Engine Error: {e}[/bold red]")
        raise typer.Exit(code=1)

@app.command("report-advanced")
def report_advanced(
    strategy: str = typer.Argument(..., help="The name of the strategy to pull advanced stat metrics for."),
    version: str = typer.Option("1.0.0", help="The version of the strategy."),
    mc_iterations: int = typer.Option(1000, help="Number of Monte Carlo resampling iterations.")
):
    """
    Retrieve Phase 5 Deep Statistical Analysis directly from the C++ Engine via Cython.
    Calculates Kelly Fractional Ratios, Monte Carlo Distributions, and Max Drawdowns.
    """
    db_path = Path(f"server/backtests/{strategy}_v{version}/crucible.duckdb")
    
    if not db_path.exists():
        console.print(f"[yellow]No backtest data found for {strategy} v{version}.[/yellow]")
        console.print(f"Run 'quanuxctl crucible start {strategy} --version {version}' to generate metrics.")
        raise typer.Exit(code=0)

    try:
        import sys
        repo_root = Path(__file__).resolve().parent.parent.parent.parent.parent
        engine_path = repo_root / "QuanuX-Backtesting-Engine" / "python"
        if str(engine_path) not in sys.path:
            sys.path.insert(0, str(engine_path))
        import quanux_backtest.cython.quanux_crucible as qc
    except ImportError as e:
        console.print(f"[bold red]Failed to load Cython Engine Bindings: {e}[/bold red]")
        raise typer.Exit(code=1)
        
    try:
        feeder = qc.PyDuckDBFeeder(str(db_path))
        metrics_json = feeder.get_metrics_json_advanced(strategy, mc_iterations)
        
        console.print(f"[bold magenta]Crucible Advanced Second-Round Diagnostics for {strategy} v{version}:[/bold magenta]")
        console.print(f"[dim]Monte Carlo Iterations: {mc_iterations}[/dim]")
        console.print(metrics_json)
        
    except Exception as e:
        console.print(f"[bold red]DuckDB Advanced Engine Error: {e}[/bold red]")
        raise typer.Exit(code=1)
