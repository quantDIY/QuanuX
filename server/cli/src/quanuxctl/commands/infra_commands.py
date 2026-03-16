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

if __name__ == "__main__":
    app()

