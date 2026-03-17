import typer
from rich.console import Console
import keyring
import subprocess
import json
import os

app = typer.Typer(help="QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning")
console = Console()

SERVICE_NAME = "quanux_terraform"
TOKEN_KEY = "do_token"

def check_provider(provider: str):
    if provider.lower() not in ["do", "gcp"]:
        console.print(f"[bold red]FATAL:[/bold red] Unsupported provider '{provider}'. Must be 'do' or 'gcp'.")
        raise typer.Exit(code=1)

@app.command("set-token")
def set_token(token: str = typer.Argument(..., help="DigitalOcean API Token")):
    """
    Securely saves the DigitalOcean API Token to the native OS Keyring (Zero-Disk storage).
    """
    try:
        keyring.set_password(SERVICE_NAME, TOKEN_KEY, token)
        console.print("[bold green]SUCCESS:[/bold green] DigitalOcean token securely locked into the OS Keychain.")
        console.print("[dim]The token will never be stored in plain text or written to disk.[/dim]")
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to inject token into the keyring: {e}")
        raise typer.Exit(code=1)

@app.command("set-spaces-keys")
def set_spaces_keys(
    access_id: str = typer.Argument(..., help="DigitalOcean Spaces Access Key ID"),
    secret_key: str = typer.Argument(..., help="DigitalOcean Spaces Secret Key")
):
    """
    Securely saves the DigitalOcean Spaces keys to the native OS Keyring.
    """
    try:
        keyring.set_password(SERVICE_NAME, "spaces_access_id", access_id)
        keyring.set_password(SERVICE_NAME, "spaces_secret_key", secret_key)
        console.print("[bold green]SUCCESS:[/bold green] DigitalOcean Spaces keys securely locked into the OS Keychain.")
    except Exception as e:
        console.print(f"[bold red]FATAL:[/bold red] Failed to inject spaces keys into the keyring: {e}")
        raise typer.Exit(code=1)

@app.command("auth-shell")
def auth_shell():
    """
    Retrieves the DigitalOcean token from the OS Keyring and outputs an export command.
    Usage: eval $(quanuxctl infra auth-shell)
    """
    try:
        token = keyring.get_password(SERVICE_NAME, TOKEN_KEY)
        if not token:
            console.print("echo '[ERROR] DO Token not found in Keyring. Run: quanuxctl infra set-token <TOKEN>'", err=True)
            raise typer.Exit(code=1)
        
        # Native MacOS IP fetching via curl
        import subprocess
        import os
        ipv4_proc = subprocess.run(["curl", "-4", "-s", "ifconfig.me"], capture_output=True, text=True)
        admin_ipv4 = ipv4_proc.stdout.strip()

        ssh_pub_path = os.path.expanduser("~/.ssh/id_ed25519.pub")
        ssh_proc = subprocess.run(["ssh-keygen", "-E", "md5", "-lf", ssh_pub_path], capture_output=True, text=True)
        fingerprint = ""
        if ssh_proc.returncode == 0:
            fingerprint = ssh_proc.stdout.split()[1].replace("MD5:", "").strip()

        print(f"export TF_VAR_do_token={token}")
        if admin_ipv4:
            print(f"export TF_VAR_admin_ip={admin_ipv4}")
        else:
            console.print("echo '[WARNING] Failed to fetch IPv4 admin IP.'", err=True)
            
        if fingerprint:
            print(f"export TF_VAR_ssh_keys='[\"{fingerprint}\"]'")
        else:
            console.print("echo '[WARNING] Failed to extract local SSH fingerprint.'", err=True)

        spaces_access = keyring.get_password(SERVICE_NAME, "spaces_access_id")
        spaces_secret = keyring.get_password(SERVICE_NAME, "spaces_secret_key")
        if spaces_access and spaces_secret:
            print(f"export SPACES_ACCESS_KEY_ID={spaces_access}")
            print(f"export SPACES_SECRET_ACCESS_KEY={spaces_secret}")

    except Exception as e:
        console.print(f"echo '[FATAL] Keyring retrieval failed: {e}'", err=True)
def get_terraform_cwd():
    current_dir = os.path.abspath(os.path.dirname(__file__))
    repo_root = os.path.abspath(os.path.join(current_dir, "../../../../../"))
    
    possible_paths = [
        os.path.join(repo_root, "QuanuX-Infra/terraform"),
        os.path.expanduser("~/Antigravity/QuanuX/QuanuX/QuanuX-Infra/terraform")
    ]
    for p in possible_paths:
        if os.path.exists(p) and os.path.isdir(p):
            return os.path.abspath(p)
    return None

def get_annex_dir():
    current_dir = os.path.abspath(os.path.dirname(__file__))
    repo_root = os.path.abspath(os.path.join(current_dir, "../../../../../"))
    
    possible_paths = [
        os.path.join(repo_root, "QuanuX-Annex"),
        os.path.expanduser("~/Antigravity/QuanuX/QuanuX/QuanuX-Annex")
    ]
    for p in possible_paths:
        if os.path.exists(p) and os.path.isdir(p):
            return os.path.abspath(p)
    return None

@app.command("do-droplets")
def do_droplets():
    """List active DigitalOcean Droplets from Terraform State."""
    cwd = get_terraform_cwd()
    if not cwd:
        console.print("[red]Could not locate QuanuX-Infra/terraform directory.[/red]")
        raise typer.Exit(1)
        
    res = subprocess.run(["terraform", "output", "-json"], cwd=cwd, capture_output=True, text=True)
    if res.returncode != 0:
        console.print("[red]Failed to read terraform outputs.[/red]")
        raise typer.Exit(1)
        
    try:
        outputs = json.loads(res.stdout)
        nexus_pub = outputs.get("quanux_panopticon_nexus_public_ip", {}).get("value", "N/A")
        nexus_int = outputs.get("quanux_panopticon_nexus_internal_ip", {}).get("value", "N/A")
        annex_pub = outputs.get("quanux_annex_node_public_ip", {}).get("value", "N/A")
        annex_int = outputs.get("quanux_annex_node_internal_ip", {}).get("value", "N/A")

        console.print("\n[bold cyan]=== DigitalOcean QuanuX Droplets ===[/bold cyan]")
        console.print(f"[bold green]Panopticon Nexus:[/bold green] {nexus_pub} (Internal: {nexus_int})")
        console.print(f"[bold green]Annex Ingestion Node:[/bold green] {annex_pub} (Internal: {annex_int})\n")
    except Exception as e:
        console.print(f"[red]Error parsing terraform output: {e}[/red]")

@app.command("do-spaces")
def do_spaces():
    """List active DigitalOcean Spaces from Terraform State."""
    cwd = get_terraform_cwd()
    if not cwd:
        console.print("[red]Could not locate QuanuX-Infra/terraform directory.[/red]")
        raise typer.Exit(1)
        
    res = subprocess.run(["terraform", "output", "-json"], cwd=cwd, capture_output=True, text=True)
    if res.returncode != 0:
        console.print("[red]Failed to read terraform outputs.[/red]")
        raise typer.Exit(1)
        
    try:
        outputs = json.loads(res.stdout)
        vault_name = outputs.get("quanux_zarr_vault_name", {}).get("value", "N/A")
        vault_endpoint = outputs.get("quanux_zarr_vault_endpoint", {}).get("value", "N/A")
        
        console.print("\n[bold cyan]=== DigitalOcean QuanuX Spaces ===[/bold cyan]")
        console.print(f"[bold green]Zarr Vault Name:[/bold green] {vault_name}")
        console.print(f"[bold green]Zarr Vault Endpoint:[/bold green] {vault_endpoint}\n")
    except Exception as e:
        console.print(f"[red]Error parsing terraform output: {e}[/red]")

@app.command("ingest-start")
def ingest_start(
    provider: str = typer.Option("do", help="Cloud provider (do or gcp)"),
    memory_limit_mb: int = typer.Option(500, help="Memory limit in MB for JetStream batching")
):
    """Starts the QuanuX asynchronous ingestion pipeline."""
    check_provider(provider)
    if provider.lower() == "gcp":
        console.print(f"[bold cyan]GCP Ingestion:[/bold cyan] Initiating pipeline with {memory_limit_mb}MB limit.")
        annex_dir = get_annex_dir()
        if not annex_dir:
            console.print("[red]Error: Could not dynamically resolve QuanuX-Annex path.[/red]")
            raise typer.Exit(code=1)
            
        pipeline_script = os.path.join(annex_dir, "gcp_ingestion_pipeline.py")
        if os.path.exists(pipeline_script):
            console.print(f"Running: python {pipeline_script}")
            subprocess.run(["python", pipeline_script])
        else:
            console.print(f"[red]Error: Pipeline script not found at {pipeline_script}[/red]")
            raise typer.Exit(code=1)
    else:
        console.print("[dim]DigitalOcean ingestion not yet implemented in this view.[/dim]")

@app.command("table-register")
def table_register(
    provider: str = typer.Option("do", help="Cloud provider (do or gcp)"),
    project: str = typer.Option(..., help="GCP Project ID"),
    uri: str = typer.Option(..., help="GCS URI for Parquet files")
):
    """Registers an external table against the data lake."""
    check_provider(provider)
    if provider.lower() == "gcp":
        console.print(f"[bold cyan]GCP BigQuery:[/bold cyan] Registering external table for {uri} in project {project}.")
        annex_dir = get_annex_dir()
        if not annex_dir:
            console.print("[red]Error: Could not dynamically resolve QuanuX-Annex path.[/red]")
            raise typer.Exit(code=1)
            
        setup_script = os.path.join(annex_dir, "gcp_bigquery_setup.py")
        if os.path.exists(setup_script):
            subprocess.run(["python", setup_script, "--project", project, "--uri", uri])
        else:
            console.print(f"[red]Error: BigQuery setup script not found at {setup_script}[/red]")
            raise typer.Exit(code=1)
    else:
        console.print("[dim]DigitalOcean table registration not applicable.[/dim]")

@app.command("nodes")
def list_nodes(provider: str = typer.Option("do", help="Cloud provider (do or gcp)")):
    """List active QuanuX nodes."""
    check_provider(provider)
    if provider.lower() == "do":
        # Route to do_droplets equivalent
        do_droplets()
    elif provider.lower() == "gcp":
        console.print("\n[bold cyan]=== GCP QuanuX Nodes ===[/bold cyan]")
        console.print("[dim]Fetching GCP Compute Engine instances... (Not yet implemented)[/dim]\n")

gcp_sql_app = typer.Typer(help="GCP Bounded AST SQL Transpilation")
app.add_typer(gcp_sql_app, name="gcp-sql")

def _get_transpiler():
    import sys
    annex_dir = get_annex_dir()
    if not annex_dir:
        return None, None
    if annex_dir not in sys.path:
        sys.path.insert(0, annex_dir)
    try:
        from gcp_transpiler import QuanuXDuckToBQTranspiler, TranspilationError
        return QuanuXDuckToBQTranspiler(), TranspilationError
    except ImportError:
        return None, None

def _fingerprint_query(query: str) -> str:
    import hashlib
    # Normalize query: uppercase, strip extra spaces
    normalized = " ".join(query.strip().upper().split())
    return hashlib.sha256(normalized.encode()).hexdigest()

def _emit_json(payload: dict, exit_code: int = 0):
    print(json.dumps(payload))
    raise typer.Exit(code=exit_code)

def _emit_human_error(error_type: str, construct: str, reason: str, fallback: str, exit_code: int = 1):
    console.print(f"\n[bold red]FATAL: {error_type}[/bold red]")
    if construct:
        console.print(f"[bold yellow]Rejected Construct:[/bold yellow] {construct}")
    console.print(f"[bold yellow]Violated Rule:[/bold yellow] {reason}")
    if fallback:
        console.print(f"\n[dim]{fallback}[/dim]\n")
    raise typer.Exit(code=exit_code)

def _resolve_gcp_runtime(output_json: bool, fingerprint: str):
    import sys
    project_id = None
    cred_path = None
    
    # Canonical Resolution Order: 1. OS Keyring (via SecretsInterface), 2. Environment Variables
    current_dir = os.path.abspath(os.path.dirname(__file__))
    repo_root = os.path.abspath(os.path.join(current_dir, "../../../../../"))
    if repo_root not in sys.path:
        sys.path.insert(0, repo_root)
        
    try:
        from server.security.secrets import SecretsInterface
        secrets = SecretsInterface()
        project_id = secrets.get_secret("GCP_PROJECT_ID")
        cred_path = secrets.get_secret("GOOGLE_APPLICATION_CREDENTIALS")
    except Exception:
        pass
        
    # Fallback to pure ENV
    if not project_id:
        project_id = os.environ.get("GCP_PROJECT_ID")
    if not cred_path:
        cred_path = os.environ.get("GOOGLE_APPLICATION_CREDENTIALS")
        
    if cred_path:
        os.environ["GOOGLE_APPLICATION_CREDENTIALS"] = cred_path

    if not project_id:
        msg = "Missing GCP_PROJECT_ID. Missing target project context."
        if output_json:
            _emit_json({"mode": "execute", "status": "error", "error_type": "ConfigError", "rejected_construct": "GCP_PROJECT_ID", "violated_rule": msg, "fallback_instruction": "Use `quanuxctl secrets set GCP_PROJECT_ID` or set ENV var.", "query_fingerprint": fingerprint}, exit_code=2)
        else:
            _emit_human_error("ConfigError", "GCP_PROJECT_ID", msg, "Use `quanuxctl secrets set GCP_PROJECT_ID` or set ENV var.", exit_code=2)

    try:
        from google.cloud import bigquery
        from google.auth.exceptions import DefaultCredentialsError
        # Provide explicit explicit project to test auth at init
        client = bigquery.Client(project=project_id)
        return client, project_id
    except Exception as e:
        msg = f"Failed to authenticate BigQuery client: {e}"
        if output_json:
            _emit_json({"mode": "execute", "status": "error", "error_type": "AuthError", "rejected_construct": "GOOGLE_APPLICATION_CREDENTIALS", "violated_rule": msg, "fallback_instruction": "Use `quanuxctl secrets set GOOGLE_APPLICATION_CREDENTIALS` or set ENV var correctly.", "query_fingerprint": fingerprint}, exit_code=2)
        else:
            _emit_human_error("AuthError", "GOOGLE_APPLICATION_CREDENTIALS", msg, "Use `quanuxctl secrets set GOOGLE_APPLICATION_CREDENTIALS` or set ENV var correctly.", exit_code=2)

@gcp_sql_app.command("validate")
def gcp_validate(
    query: str = typer.Argument(..., help="DuckDB SQL Query to validate"),
    output_json: bool = typer.Option(False, "--json", help="Emit purely JSON payload for machine execution")
):
    """Validates if the query is within the approved Phase 1 matrix."""
    fingerprint = _fingerprint_query(query)
    transpiler, TranspilationErrorCls = _get_transpiler()
    if not transpiler:
        if output_json: _emit_json({"mode": "validate", "status": "error", "error_type": "InternalError", "violated_rule": "Missing transpiler", "query_fingerprint": fingerprint}, 1)
        raise typer.Exit(1)
        
    try:
        transpiler.transpile(query)
        if output_json:
            _emit_json({
                "mode": "validate", "status": "success", "query_fingerprint": fingerprint,
                "rule_surface_version": "tract2_phase1"
            })
        else:
            console.print("[bold green]SUCCESS:[/bold green] Query is within the approved Phase 1 bounded matrix.")
            
    except TranspilationErrorCls as e:
        if output_json:
            _emit_json({"mode": "validate", "status": "error", "error_type": "TranspilationError", "rejected_construct": e.construct, "violated_rule": e.reason, "fallback_instruction": e.fallback, "query_fingerprint": fingerprint}, exit_code=1)
        else:
            _emit_human_error("Prototype Matrix Boundary Violation", e.construct, e.reason, e.fallback, exit_code=1)

@gcp_sql_app.command("transpile")
def gcp_transpile(
    query: str = typer.Argument(..., help="DuckDB SQL Query to transpile"),
    output_json: bool = typer.Option(False, "--json", help="Emit purely JSON payload for machine execution")
):
    """Emits translated BigQuery SQL if within the approved Phase 1 matrix."""
    fingerprint = _fingerprint_query(query)
    transpiler, TranspilationErrorCls = _get_transpiler()
    
    try:
        bq_sql = transpiler.transpile(query)
        if output_json:
            _emit_json({
                "mode": "transpile", "status": "success", "query_fingerprint": fingerprint,
                "rule_surface_version": "tract2_phase1", "sql": bq_sql
            })
        else:
            console.print("[bold cyan]BigQuery Standard SQL (Translated):[/bold cyan]")
            console.print(f"{bq_sql}")
            
    except TranspilationErrorCls as e:
        if output_json:
            _emit_json({"mode": "transpile", "status": "error", "error_type": "TranspilationError", "rejected_construct": e.construct, "violated_rule": e.reason, "fallback_instruction": e.fallback, "query_fingerprint": fingerprint}, exit_code=1)
        else:
            _emit_human_error("Prototype Matrix Boundary Violation", e.construct, e.reason, e.fallback, exit_code=1)

@gcp_sql_app.command("execute")
def gcp_execute(
    query: str = typer.Argument(..., help="DuckDB SQL Query to execute"),
    max_rows: int = typer.Option(100, help="Maximum rows to fetch remotely"),
    dry_run: bool = typer.Option(False, help="Validate and transpile only, do not send to GCP"),
    timeout: int = typer.Option(30, help="Timeout in seconds for remote execution"),
    output_json: bool = typer.Option(False, "--json", help="Emit purely JSON payload for machine execution")
):
    """Validates, transpiles, and executes bounded SQL against BigQuery."""
    fingerprint = _fingerprint_query(query)
    
    if max_rows <= 0 or timeout <= 0:
        msg = f"Invalid bounds. Max rows ({max_rows}) and timeout ({timeout}) must be positive integers."
        if output_json:
            _emit_json({"mode": "execute", "status": "error", "error_type": "RuntimeError", "rejected_construct": "BOUNDS", "violated_rule": msg, "fallback_instruction": "Provide positive bounds.", "query_fingerprint": fingerprint}, exit_code=3)
        else:
            _emit_human_error("RuntimeError", "BOUNDS", msg, "Provide positive bounds.", exit_code=3)

    transpiler, TranspilationErrorCls = _get_transpiler()
    
    try:
        bq_sql = transpiler.transpile(query)
        if dry_run:
            if output_json:
                _emit_json({
                    "mode": "execute_dry_run", "status": "success", "query_fingerprint": fingerprint,
                    "rule_surface_version": "tract2_phase1", "bounds": {"max_rows": max_rows, "timeout": timeout},
                    "row_count": 0, "sql": bq_sql
                })
            else:
                console.print(f"[bold yellow]DRY-RUN:[/bold yellow] Validation successful. Query would execute as (Max Rows: {max_rows}, Timeout: {timeout}s):")
                console.print(f"{bq_sql}")
            return
            
        # Stop execution cleanly and immediately securely without Python tracebacks bleeding.
        client, project_id = _resolve_gcp_runtime(output_json, fingerprint)
        
        if not output_json:
            console.print(f"[dim]Executing bounded query (Max Rows: {max_rows}, Timeout: {timeout}s)...[/dim]")
            
        try:
            table = transpiler.execute_bounded(client, bq_sql, timeout=timeout, max_results=max_rows)
        except Exception as exec_e:
            msg = f"Remote BigQuery error: {exec_e}"
            if output_json:
                _emit_json({"mode": "execute", "status": "error", "error_type": "ExecutionError", "rejected_construct": "REMOTE", "violated_rule": msg, "fallback_instruction": "Check GCP syntax parity manually.", "query_fingerprint": fingerprint}, exit_code=4)
            else:
                _emit_human_error("ExecutionError", "REMOTE", msg, "Check GCP syntax parity manually.", exit_code=4)
            
        row_count = table.num_rows if table else 0
        
        if output_json:
            _emit_json({
                "mode": "execute", "status": "success", "query_fingerprint": fingerprint,
                "rule_surface_version": "tract2_phase1", "bounds": {"max_rows": max_rows, "timeout": timeout},
                "row_count": row_count, "sql": bq_sql
            })
            return
            
        console.print("[bold green]SUCCESS:[/bold green] Bounded execution complete.")
        console.print(f"[bold cyan]Retrieved {row_count} rows.[/bold cyan]")
        
        if row_count > 0:
            from rich.table import Table
            rich_table = Table(show_header=True, header_style="bold magenta")
            for name in table.column_names:
                rich_table.add_column(name)
                
            for i in range(table.num_rows):
                row_data = [str(table.column(c)[i].as_py()) for c in table.column_names]
                rich_table.add_row(*row_data)
                
            console.print(rich_table)
        
    except TranspilationErrorCls as e:
        if output_json:
            _emit_json({"mode": "execute", "status": "error", "error_type": "TranspilationError", "rejected_construct": e.construct, "violated_rule": e.reason, "fallback_instruction": e.fallback, "query_fingerprint": fingerprint}, exit_code=1)
        else:
            _emit_human_error("Prototype Matrix Boundary Violation", e.construct, e.reason, e.fallback, exit_code=1)

if __name__ == "__main__":
    app()
