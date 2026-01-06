import typer
from rich import print
from rich.prompt import Prompt
from rich.table import Table
from rich.console import Console
from server.security.secrets import KeyringBackend

app = typer.Typer()
console = Console()

# Defined keys we prompt for in 'setup'
SECRETS_MAP = [
    ("OPENAI_API_KEY", "OpenAI API Key (sk-...)"),
    ("QUANUX_GEMINI_API_KEY", "Google Gemini API Key (AIza...)"),
    ("TOPSTEP__USERNAME", "Topstep Username"),
    ("TOPSTEP__PASSWORD", "Topstep Password"),
    ("TOPSTEP__API_KEY", "Topstep API Key"),
    ("TRADINGVIEW_CLIENT_ID", "TradingView Client ID"),
    ("BROKER_API_KEY", "Broker API Key"),
    # Rithmic (Generic & Prop Firms)
    ("QUANUX_RITHMIC_USER", "Rithmic (Generic) Username"),
    ("QUANUX_RITHMIC_PASS", "Rithmic (Generic) Password"),
    ("QUANUX_RITHMIC_SYSTEM", "Rithmic (Generic) System"),
    ("QUANUX_RITHMIC_URL", "Rithmic (Generic) WSS URL"),
    ("QUANUX_RITHMIC_APP_NAME", "Rithmic (Generic) App Name"),
    ("QUANUX_RITHMIC_APP_VERSION", "Rithmic (Generic) App Version"),

    # Brokers
    ("QUANUX_IRONBEAM_USER", "Ironbeam Username"),
    ("QUANUX_IRONBEAM_PASS", "Ironbeam Password"),
    ("QUANUX_IRONBEAM_SYSTEM", "Ironbeam System (e.g. Rithmic Test)"),
    ("QUANUX_IRONBEAM_URL", "Ironbeam WSS URL"),
    ("QUANUX_IRONBEAM_APP_NAME", "Ironbeam App Name"),
    ("QUANUX_IRONBEAM_APP_VERSION", "Ironbeam App Version"),

    ("QUANUX_AMP_USER", "Amp Futures Username"),
    ("QUANUX_AMP_PASS", "Amp Futures Password"),
    ("QUANUX_AMP_SYSTEM", "Amp System"),
    ("QUANUX_AMP_URL", "Amp WSS URL"),
    ("QUANUX_AMP_APP_NAME", "Amp App Name"),
    ("QUANUX_AMP_APP_VERSION", "Amp App Version"),

    ("QUANUX_DISCOUNT_USER", "Discount Trading Username"),
    ("QUANUX_DISCOUNT_PASS", "Discount Trading Password"),
    ("QUANUX_DISCOUNT_SYSTEM", "Discount System"),
    ("QUANUX_DISCOUNT_URL", "Discount WSS URL"),
    ("QUANUX_DISCOUNT_APP_NAME", "Discount App Name"),
    ("QUANUX_DISCOUNT_APP_VERSION", "Discount App Version"),

    # Prop Firms
    ("QUANUX_APEX_USER", "Apex Trader Username"),
    ("QUANUX_APEX_PASS", "Apex Trader Password"),
    ("QUANUX_APEX_SYSTEM", "Apex System (e.g. Apex)"),
    ("QUANUX_APEX_URL", "Apex WSS URL"),
    ("QUANUX_APEX_APP_NAME", "Apex App Name"),
    ("QUANUX_APEX_APP_VERSION", "Apex App Version"),

    ("QUANUX_ELITE_USER", "Elite Trader Funding Username"),
    ("QUANUX_ELITE_PASS", "Elite Trader Funding Password"),
    ("QUANUX_ELITE_SYSTEM", "Elite System"),
    ("QUANUX_ELITE_URL", "Elite WSS URL"),
    ("QUANUX_ELITE_APP_NAME", "Elite App Name"),
    ("QUANUX_ELITE_APP_VERSION", "Elite App Version"),

    ("QUANUX_TAKEPROFIT_USER", "Take Profit Trader Username"),
    ("QUANUX_TAKEPROFIT_PASS", "Take Profit Trader Password"),
    ("QUANUX_TAKEPROFIT_SYSTEM", "Take Profit System"),
    ("QUANUX_TAKEPROFIT_URL", "Take Profit WSS URL"),
    ("QUANUX_TAKEPROFIT_APP_NAME", "Take Profit App Name"),
    ("QUANUX_TAKEPROFIT_APP_VERSION", "Take Profit App Version"),

    # Charting (Placeholder)
    # TradingView ID already exists above

    # Trading Platforms (Placeholders)
    ("QUANUX_EDGEPROX_USER", "EdgeProX Username"),
    ("QUANUX_EDGEPROX_PASS", "EdgeProX Password"),

    ("QUANUX_MOTIVEWAVE_USER", "MotiveWave Username"),
    ("QUANUX_MOTIVEWAVE_PASS", "MotiveWave Password"),

    ("QUANUX_SIERRA_USER", "Sierra Chart Username"),
    ("QUANUX_SIERRA_PASS", "Sierra Chart Password"),

    ("QUANUX_JIGSAW_USER", "Jigsaw Trading Username"),
    ("QUANUX_JIGSAW_PASS", "Jigsaw Trading Password"),

    ("QUANUX_QUANTOWER_USER", "Quantower Username"),
    ("QUANUX_QUANTOWER_PASS", "Quantower Password"),

    ("QUANUX_INVESTORRT_USER", "Investor/RT Username"),
    ("QUANUX_INVESTORRT_PASS", "Investor/RT Password"),

    ("QUANUX_MULTICHARTS_USER", "MultiCharts Username"),
    ("QUANUX_MULTICHARTS_PASS", "MultiCharts Password"),

    ("QUANUX_TRADENAV_USER", "Trade Navigator Username"),
    ("QUANUX_TRADENAV_PASS", "Trade Navigator Password"),

    ("QUANUX_BOOKMAP_USER", "Bookmap Username"),
    ("QUANUX_BOOKMAP_PASS", "Bookmap Password"),
]

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
def set(key: str, value: str):
    """
    Set a specific secret key.
    """
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
