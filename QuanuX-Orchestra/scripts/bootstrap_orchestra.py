import urllib.request
import os
import sys

FIX_REPO_URL = "https://raw.githubusercontent.com/FIXTradingCommunity/orchestrations/master/FIX%20Standard/OrchestraFIXLatest.xml"
OUTPUT_DIR = "repository"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "FIX.latest.xml")

def download_orchestra():
    print(f"[*] Downloading True Instance XML from {FIX_REPO_URL}...")
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    try:
        response = urllib.request.urlopen(FIX_REPO_URL)
        data = response.read()
        with open(OUTPUT_FILE, "wb") as f:
            f.write(data)
        print(f"[bold green]Matrix Acquired.[/bold green] Standard XML saved natively to {OUTPUT_FILE}")
    except Exception as e:
        print(f"[bold red]Fetch Failed:[/bold red] The payload could not be acquired: {e}.")
        print("[bold red]FATAL: Architecture violation detected. Initiating sys.exit(1).[/bold red]")
        sys.exit(1)

if __name__ == "__main__":
    download_orchestra()
