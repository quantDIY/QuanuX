import os
import sys
import socket
import asyncio
import logging
import subprocess
from logging.handlers import RotatingFileHandler
from typing import Dict, Any, Optional
import typer
from rich.console import Console
from pydantic import BaseModel

app = typer.Typer(no_args_is_help=True)
console = Console()

# --- Logging Setup (The Black Box) ---
LOG_FILE = "/var/log/quanux/fix.log"
# Create dirs if they don't exist, though typically requires root. 
# We'll fallback to /tmp if we can't write to /var/log.
try:
    os.makedirs(os.path.dirname(LOG_FILE), exist_ok=True)
    open(LOG_FILE, 'a').close()
except (PermissionError, OSError):
    LOG_FILE = "/tmp/quanux_fix.log"

logger = logging.getLogger("quanux.probe")
logger.setLevel(logging.INFO)
# Configurable retention, wait for Fast-Nerve to configure it, defaults to 10MB, 5 backups
handler = RotatingFileHandler(LOG_FILE, maxBytes=10*1024*1024, backupCount=5)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
handler.setFormatter(formatter)
if not logger.handlers:
    logger.addHandler(handler)

# --- Pydantic Data Models ---
class ProbeResult(BaseModel):
    bone: bool
    nerve: bool
    soul: bool
    status: str
    details: Dict[str, Any]

# --- Core Logic ---
def check_bone(fix: bool, log_to_console: bool = True) -> bool:
    """The Bone (OS/Environment): Audit Conda & Path."""
    if log_to_console: console.print("[bold blue][Bone][/bold blue] Auditing Environment...")
    logger.info("[Bone] Auditing Environment...")
    success = True
    
    # 1. Conda Check
    if "CONDA_PREFIX" not in os.environ or not os.environ["CONDA_PREFIX"].endswith("quanux-node"):
        msg = "[!] Conda environment 'quanux-node' is not active."
        if log_to_console: console.print(msg)
        logger.warning(msg)
        success = False
        if fix:
            msg = "  [Auto-Suture] Attempting to activate conda 'quanux-node' (Packages)"
            if log_to_console: console.print(msg)
            logger.info(msg)
            try:
                subprocess.run(["conda", "install", "-y", "-c", "quanux-repo", "set-quanux-node"], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                success = True
            except (subprocess.CalledProcessError, FileNotFoundError):
                pass
    else:
        if log_to_console: console.print("[+] Conda 'quanux-node' is active.")
        logger.info("[+] Conda 'quanux-node' is active.")
        
    # 2. Binary Path Check
    if not any(os.access(os.path.join(p, "set-quanux-node"), os.X_OK) for p in os.environ.get("PATH", "").split(os.pathsep)):
        msg = "[!] 'set-quanux-node' binary not in PATH."
        if log_to_console: console.print(msg)
        logger.warning(msg)
        success = False
        if fix:
            msg = "  [Auto-Suture] Re-linking binaries..."
            if log_to_console: console.print(msg)
            logger.info(msg)
            try:
                os.symlink("/opt/quanux/bin/set-quanux-node", "/usr/local/bin/set-quanux-node")
                success = True
            except OSError as e:
                logger.error(f"Link failed: {e}")
    else:
        if log_to_console: console.print("[+] Binary path verified.")
        logger.info("[+] Binary path verified.")
        
    return success

def check_nerve(fix: bool, log_to_console: bool = True) -> bool:
    """The Nerve (NATS Connectivity): Audit NATS and Cluster Ports."""
    if log_to_console: console.print("[bold cyan][Nerve][/bold cyan] Auditing NATS Connectivity...")
    logger.info("[Nerve] Auditing NATS Connectivity...")
    success = False
    
    nats_local = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(1)
        if s.connect_ex(('127.0.0.1', 4222)) == 0:
            nats_local = True
            if log_to_console: console.print("[+] Local NATS (Port 4222) is active.")
        else:
            msg = "[!] Local NATS (Port 4222) is unreachable."
            if log_to_console: console.print(msg)
            logger.warning(msg)

    nats_cluster = False
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(1)
        if s.connect_ex(('0.0.0.0', 6222)) == 0 or s.connect_ex(('127.0.0.1', 6222)) == 0:
            nats_cluster = True
            if log_to_console: console.print("[+] Cluster Route (Port 6222) is active.")
        else:
            msg = "[!] Cluster Route (Port 6222) is unreachable."
            if log_to_console: console.print(msg)
            logger.warning(msg)
            
    if nats_local or nats_cluster:
        success = True
    
    if not success:
        if fix:
            msg = "  [Auto-Suture] Nerve Failure. Reloading and restarting nats-server..."
            if log_to_console: console.print(msg)
            logger.info(msg)
            try:
                subprocess.run(["sudo", "systemctl", "reload-or-restart", "nats-server"], check=True)
                success = True
            except subprocess.CalledProcessError as e:
                logger.error(f"Restart failed: {e}")
    return success

def check_soul(fix: bool, log_to_console: bool = True) -> bool:
    """The Soul (Security/Identity): Audit RSA & Ed25519 Keys."""
    if log_to_console: console.print("[bold magenta][Soul][/bold magenta] Auditing Identity & NKeys...")
    logger.info("[Soul] Auditing Identity & NKeys...")
    success = True
    
    key_files = ["/etc/quanux/keys/node_rsa", "/etc/quanux/keys/node.nk"]
    
    for kf in key_files:
        if not os.path.exists(kf):
            msg = f"[!] Identity file missing: {kf}"
            if log_to_console: console.print(msg)
            logger.warning(msg)
            success = False
            continue
            
        stat = os.stat(kf)
        mode = stat.st_mode & 0o777
        if mode != 0o600:
            msg = f"[!] Identity file {kf} has unsafe permissions: {oct(mode)} (expected 0600)"
            if log_to_console: console.print(msg)
            logger.warning(msg)
            success = False
            if fix:
                msg2 = f"  [Auto-Suture] Adjusting permissions for {kf} to 600..."
                if log_to_console: console.print(msg2)
                logger.info(msg2)
                try:
                    subprocess.run(["sudo", "chmod", "600", kf], check=True)
                    success = True
                except subprocess.CalledProcessError as e:
                    logger.error(f"Failed to chmod: {e}")
        else:
            if log_to_console: console.print(f"[+] Identity file {kf} is secured (600).")
            logger.info(f"[+] Identity file {kf} is secured (600).")
            
    return success

def execute_probe(fix: bool, log_to_console: bool = True) -> ProbeResult:
    b = check_bone(fix, log_to_console)
    n = check_nerve(fix, log_to_console)
    s = check_soul(fix, log_to_console)
    
    status = "NOMINAL"
    if not b: status = "NUTRIENT DEFICIENCY (Bone Failure)"
    if not n: status = "ISOLATED (Nerve Failure)"
    if not s: status = "IDENTITY CRISIS (Soul Failure)"
    
    # If multiple fail, Nerve or Bone usually takes precedence in exit logic.
    # We return the combination.
    if not b and not n and not s:
        status = "CRITICAL METABOLIC FAILURE"
        
    return ProbeResult(
        bone=b, nerve=n, soul=s,
        status=status,
        details={"fix_attempted": fix}
    )

@app.command("probe")
def probe_cmd(
    fix: bool = typer.Option(False, "--fix", help="Execute Auto-Suture Protocol to repair critical systems")
):
    """
    Neural Tap: The cluster diagnostic stethoscope and surgical kit.
    Assesses Sovereign status across Bone (OS), Nerve (Networking), and Soul (Identity).
    """
    console.print(f"[bold]{'Executing Neural Tap & Auto-Suture' if fix else 'Executing Neural Tap Diagnostic'}[/bold]...")
    
    result = execute_probe(fix=fix, log_to_console=True)
    
    if result.bone and result.nerve and result.soul:
        console.print(f"\n[bold green]Sovereign Status: {result.status}[/bold green]")
        raise typer.Exit(code=0)
    elif not result.nerve:
        console.print(f"\n[bold red]Sovereign Status: {result.status}[/bold red]")
        raise typer.Exit(code=1)
    elif not result.bone:
        console.print(f"\n[bold red]Sovereign Status: {result.status}[/bold red]")
        raise typer.Exit(code=2)
    elif not result.soul:
        console.print(f"\n[bold red]Sovereign Status: {result.status}[/bold red]")
        raise typer.Exit(code=3)

if __name__ == "__main__":
    app()
