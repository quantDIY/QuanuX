#!/usr/bin/env python3
import urllib.request
import os

FIX_REPO_URL = "https://raw.githubusercontent.com/FIXTradingCommunity/orchestra/master/repository/FIX.latest.xml"
OUTPUT_DIR = "QuanuX-Orchestra/repository"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "FIX.latest.xml")

def download_orchestra():
    print(f"[*] Downloading pure FIX Orchestra XML from {FIX_REPO_URL}...")
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    try:
        response = urllib.request.urlopen(FIX_REPO_URL)
        data = response.read()
        with open(OUTPUT_FILE, "wb") as f:
            f.write(data)
        print(f"[bold green]Matrix Acquired.[/bold green] Standard XML saved natively to {OUTPUT_FILE}")
    except Exception as e:
        print(f"[bold red]Fetch Failed:[/bold red] {e}")
        # Try local fallback if internet is blocked
        if os.path.exists("QuanuX-Orchestra/scripts/fix_repository.xml"):
             with open("QuanuX-Orchestra/scripts/fix_repository.xml", "rb") as f:
                  data = f.read()
                  with open(OUTPUT_FILE, "wb") as out:
                      out.write(data)
             print(f"[yellow]Local Mockup Matrix Utilized[/yellow] at {OUTPUT_FILE}")

if __name__ == "__main__":
    download_orchestra()
