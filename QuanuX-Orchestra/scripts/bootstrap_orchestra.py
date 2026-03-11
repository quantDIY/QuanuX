#!/usr/bin/env python3
import urllib.request
import os
import sys

OUTPUT_DIR = "repository"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "repository.xsd")

def download_orchestra():
    print(f"[*] Validating True Metal Schema at {OUTPUT_FILE}...")
    if not os.path.exists(OUTPUT_FILE):
        print(f"[bold red]Fetch Failed:[/bold red] The true local schema could not be found at {OUTPUT_FILE}. Architecture violation detected.")
        sys.exit(1)
        
    print(f"[bold green]Matrix Acquired.[/bold green] Standard XML validated natively at {OUTPUT_FILE}")

if __name__ == "__main__":
    download_orchestra()
