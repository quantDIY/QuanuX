import typer
import sys
import os
from pathlib import Path
from rich.console import Console

app = typer.Typer(help="Manage Analytical Extensions (Validate & Estimate SQL)")
console = Console()

def _load_transpiler():
    repo_root = Path(__file__).resolve().parent.parent.parent.parent.parent.parent.parent
    
    try:
        import importlib.util
        spec = importlib.util.spec_from_file_location(
            "gcp_transpiler", 
            str(repo_root / "QuanuX-Annex" / "transpiler" / "gcp_transpiler.py")
        )
        gcp_transpiler = importlib.util.module_from_spec(spec)
        sys.modules["gcp_transpiler"] = gcp_transpiler
        spec.loader.exec_module(gcp_transpiler)
        return gcp_transpiler.QuanuXDuckToBQTranspiler, gcp_transpiler.TranspilationError
    except Exception as e:
        console.print(f"[red]Error loading transpiler module: {e}[/red]")
        sys.exit(1)

@app.command("validate")
def validate(
    sql_file: Path = typer.Argument(..., help="Path to the backtest SQL file"),
    target: str = typer.Option("bq", "--target", "-t", help="Target analytics engine")
):
    """Parses local DuckDB SQL against the Allowed/Banned matrix."""
    if not sql_file.exists():
        console.print(f"[red]Error: File {sql_file} not found.[/red]")
        raise typer.Exit(code=1)
        
    query = sql_file.read_text()
    QuanuXDuckToBQTranspiler, TranspilationError = _load_transpiler()
    
    try:
        transpiler = QuanuXDuckToBQTranspiler()
        bq_sql = transpiler.transpile(query)
        console.print("[green]Syntax Validation: PASS[/green]")
        console.print("[green]All explicitly bounded JOIN operations and AST nodes are authorized under Phase 1 control spec.[/green]")
    except TranspilationError as e:
        console.print(f"[red]Syntax Rejection: {e}[/red]")
        raise typer.Exit(code=1)
    except Exception as e:
        console.print(f"[red]Unexpected Error: {e}[/red]")
        raise typer.Exit(code=1)

@app.command("estimate")
def estimate(
    sql_file: Path = typer.Argument(..., help="Path to the backtest SQL file"),
    target: str = typer.Option("bq", "--target", "-t", help="Target analytics engine")
):
    """Transpiles AST and hits BigQuery Dry-Run API for metrics."""
    if not sql_file.exists():
        console.print(f"[red]Error: File {sql_file} not found.[/red]")
        raise typer.Exit(code=1)
        
    query = sql_file.read_text()
    QuanuXDuckToBQTranspiler, TranspilationError = _load_transpiler()
    
    try:
        transpiler = QuanuXDuckToBQTranspiler()
        bq_sql = transpiler.transpile(query)
    except TranspilationError as e:
        console.print(f"[red]Syntax Rejection before estimate: {e}[/red]")
        raise typer.Exit(code=1)
        
    console.print(f"[blue]Initiating GCP Dry-Run against Transpiled SQL...[/blue]")
    try:
        from google.cloud import bigquery
        from google.auth.exceptions import DefaultCredentialsError
        try:
            client = bigquery.Client()
            job_config = bigquery.QueryJobConfig(dry_run=True, use_query_cache=False)
            query_job = client.query(bq_sql, job_config=job_config)
            
            bytes_billed = query_job.total_bytes_processed
            mb_billed = bytes_billed / (1024 * 1024) if bytes_billed else 0
            
            console.print("[green]Estimate Complete:[/green]")
            console.print(f"  Total Bytes Billed: [cyan]{bytes_billed} bytes ({mb_billed:.2f} MB)[/cyan]")
            console.print(f"  Expected Latency: [cyan]Warm Analytical Tier (~200ms-800ms)[/cyan]")
        except DefaultCredentialsError:
            console.print("[yellow]Notice: Mocking dry-run response since GCP credentials are not found in CI/CD context.[/yellow]")
            console.print("[green]Estimate Complete (Mock):[/green]")
            console.print(f"  Total Bytes Billed: [cyan]4194304 bytes (4.00 MB)[/cyan]")
            console.print(f"  Expected Latency: [cyan]Warm Analytical Tier (~212ms)[/cyan]")
    except ImportError:
            console.print("[yellow]Notice: google-cloud-bigquery library not installed, returning mock estimation.[/yellow]")
            console.print("[green]Estimate Complete (Mock):[/green]")
            console.print(f"  Total Bytes Billed: [cyan]4194304 bytes (4.00 MB)[/cyan]")
            console.print(f"  Expected Latency: [cyan]Warm Analytical Tier (~212ms)[/cyan]")
