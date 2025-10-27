import click, subprocess, os, sys, json, time, requests

BASE = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", "..", ".."))
FLASK_APP = os.path.join(BASE, "server", "bridges", "signalr", "flask", "app.py")
HEALTH = "http://127.0.0.1:8091/bridge/signalr/health"

@click.group("bridge")
def bridge():
    """Manage SignalR bridge (scaffold)."""
    pass

@bridge.command("flask")
def run_flask():
    """Run Flask supervisor (foreground)."""
    env = os.environ.copy()
    # NOTE: in wiring, inject secrets from secrets provider
    cmd = [sys.executable, FLASK_APP]
    subprocess.call(cmd, env=env)

@bridge.command("start")
def start():
    """Start Node worker via Flask supervisor."""
    try:
        requests.post("http://127.0.0.1:8091/bridge/signalr/start", timeout=2)
        click.echo("Bridge start requested.")
    except Exception as e:
        click.echo(f"Flask not up? Run: quanuxctl bridge flask\n{e}")

@bridge.command("stop")
def stop():
    """Stop Node worker via Flask supervisor."""
    try:
        r = requests.post("http://127.0.0.1:8091/bridge/signalr/stop", timeout=2)
        click.echo(r.text)
    except Exception as e:
        click.echo(f"Flask not up? {e}")

@bridge.command("status")
def status():
    """Bridge status."""
    try:
        r = requests.get("http://127.0.0.1:8091/bridge/signalr/status", timeout=2)
        click.echo(r.text)
    except Exception as e:
        click.echo(f"Flask not up? {e}")
