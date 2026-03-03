import typer
from rich.console import Console
import keyring

app = typer.Typer(help="QuanuX Control Plane: Ephemeral Infrastructure Auth & Provisioning")
console = Console()

SERVICE_NAME = "quanux_terraform"
TOKEN_KEY = "do_token"

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

    except Exception as e:
        console.print(f"echo '[FATAL] Keyring retrieval failed: {e}'", err=True)
        raise typer.Exit(code=1)

if __name__ == "__main__":
    app()
