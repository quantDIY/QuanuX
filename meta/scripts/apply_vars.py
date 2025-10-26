#!/usr/bin/env python3
import re, sys, json
from pathlib import Path
try:
    import yaml
except Exception:
    print("Please `pip install pyyaml` in your env to run this script.")
    sys.exit(1)

ROOT = Path(__file__).resolve().parents[2]
VARS = yaml.safe_load((ROOT / "meta" / "config" / "vars.yaml").read_text())

# Flatten versions.* to top-level tokens for convenience
flat = {}
for k, v in (VARS.get("versions") or {}).items():
    flat[f"{k}_VERSION"] = str(v)
# Allow refs like {{PYTHON_VERSION}} → versions.PYTHON

TOKEN = re.compile(r"{{\s*([A-Z0-9_]+)\s*}}")

def render(path: Path):
    text = path.read_text()
    def repl(m):
        key = m.group(1)
        return flat.get(key, m.group(0))  # leave untouched if unknown
    new = TOKEN.sub(repl, text)
    if new != text:
        path.write_text(new)
        return True
    return False

def main():
    # Default globs: docs & config-like files under meta + root files
    targets = [
        ROOT / "meta" / "docs",
        ROOT / "README.md",
        ROOT / "environment.yml",
        ROOT / "server",
        ROOT / "client",
    ]
    changed = 0
    for base in targets:
        if not Path(base).exists(): 
            continue
        for p in Path(base).rglob("*"):
            if p.is_file() and p.suffix.lower() in {".md", ".yml", ".yaml", ".json"}:
                if render(p): changed += 1
    print(f"Updated {changed} file(s) with vars.")
if __name__ == "__main__":
    main()
