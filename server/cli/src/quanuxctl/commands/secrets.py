import typer
from rich import print
from rich.prompt import Prompt
from rich.table import Table
from rich.console import Console
from server.security.secrets import KeyringBackend, KNOWN_INTEGRATIONS as SECRETS_MAP

app = typer.Typer()
console = Console()



@app.command()
def setup():
    """
    Interactive wizard to set up common API keys.
    """
    console.print("[bold yellow]🔑 QuanuX Secrets Setup[/bold yellow]")
    console.print("Secrets are stored in your OS Keyring. Leave empty to skip/keep existing.\n")
    
    kb = KeyringBackend()
    if not kb._keyring:
        console.print("[bold red]❌ Keyring unavailable on this system.[/bold red]")
        raise typer.Exit(1)

    for key, label in SECRETS_MAP:
        # Check existing
        existing = kb.get(key) or kb.get(f"QUANUX_{key}")
        status = "[green]Set[/green]" if existing else "[dim]Not set[/dim]"
        
        console.print(f"👉 [bold]{label}[/bold] (Status: {status})")
        val = Prompt.ask("   Value", password=True, show_default=False, default="")
        
        if val.strip():
            full_key = key if key.startswith("QUANUX_") else f"QUANUX_{key}"
            kb.set(full_key, val)
            console.print(f"   [green]✅ Saved {full_key}[/green]")
        else:
            console.print("   [dim]Skipped[/dim]")
        print()
    
    console.print("[bold green]✨ Setup complete.[/bold green]")

@app.command()
def set(key: str, value: str = typer.Argument(None)):
    """
    Set a specific secret key. Prompts if value is missing.
    """
    if not value:
        value = Prompt.ask(f"Enter value for {key}", password=True)

    kb = KeyringBackend()
    full_key = key if key.startswith("QUANUX_") else f"QUANUX_{key}"
    try:
        kb.set(full_key, value)
        console.print(f"[green]✅ Set {full_key}[/green]")
    except Exception as e:
        console.print(f"[red]❌ Failed to set secret: {e}[/red]")

@app.command()
def get(key: str):
    """
    Retrieve and print a secret value (Show plaintext).
    """
    kb = KeyringBackend()
    full_key = key if key.startswith("QUANUX_") else f"QUANUX_{key}"
    val = kb.get(full_key)
    if val:
        console.print(f"{full_key}: [bold]{val}[/bold]")
    else:
        console.print(f"[yellow]Secret {full_key} not found.[/yellow]")

@app.command()
def list_keys():
    """
    List configured keys (hiding values).
    """
    # Keyring doesn't universally support listing keys unfortunately.
    # We can only check the ones we know about.
    table = Table(title="Known Secrets Status")
    table.add_column("Key", style="cyan")
    table.add_column("Status", style="green")

    kb = KeyringBackend()
    for key, label in SECRETS_MAP:
        full_key = key if key.startswith("QUANUX_") else f"QUANUX_{key}"
        val = kb.get(full_key)
        status = "✅ Set" if val else "❌ Missing"
        table.add_row(full_key, status)
    
    console.print(table)
