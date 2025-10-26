from pathlib import Path
import json

def cmd_docs(argv):
    root = Path(".")
    index = root / "docs" / "INDEX.md"
    roadmap = root / "docs" / "ROADMAP.md"
    backlog = root / "docs" / "Backlog" / "backlog.v1.json"
    print("== QuanuX Docs ==")
    if index.exists(): print(f"- INDEX: {index}")
    if roadmap.exists(): print(f"- ROADMAP: {roadmap}")
    if backlog.exists(): print(f"- BACKLOG: {backlog}")

def cmd_backlog(argv):
    backlog = Path("docs/Backlog/backlog.v1.json")
    if not backlog.exists():
        print("No backlog found.")
        return
    data = json.loads(backlog.read_text())
    print(f"Backlog version: {data.get('version')}")
    for track in data.get("tracks", []):
        print(f"\n[{track['title']}]")
        for it in track.get("items", []):
            print(f"- {it['id']}: {it['title']} (status={it['status']})")
