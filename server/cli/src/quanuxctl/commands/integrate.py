import shutil
from pathlib import Path
import typer
from rich.console import Console

app = typer.Typer()
console = Console()

@app.command()
def onixs(
    sdk_path: Path = typer.Argument(..., exists=True, file_okay=False, dir_okay=True, help="Path to the OnixS SDK root directory"),
    dry_run: bool = typer.Option(False, "--dry-run", help="Simulate the injection without moving files")
):
    """
    Inject the OnixS C++ SDK into the QuanuX extension.
    """
    detect_and_integrate(sdk_path, "onixs", "include/OnixS/FixEngine.h", dry_run)

def detect_and_integrate(sdk_path: Path, ext_name: str, marker_file: str, dry_run: bool):
    sdk_path = sdk_path.resolve()
    console.print(f"🔍 Analyzing SDK at: [bold]{sdk_path}[/bold]")

    if (sdk_path / marker_file).exists():
        console.print(f"✅ Detected verified SDK for: [green]{ext_name}[/green]")
    else:
        console.print(f"❌ [red]Error:[/red] Could not find '{marker_file}' in the provided SDK path.")
        raise typer.Exit(code=1)

    # Calculate repo root (assuming this file is in server/cli/src/quanuxctl/commands)
    # Adjustment: logic depends on where this file effectively sits. 
    # Current: server/cli/src/quanuxctl/commands/integrate.py
    # Root is up 5 levels? 
    # Let's use a safer relative lookup if possible, or assume CWD is project root if run via python -m
    
    # Heuristic: Look for 'extensions' dir in parents
    repo_root = None
    for parent in Path(__file__).parents:
        if (parent / "extensions").exists():
            repo_root = parent
            break
    
    if not repo_root:
        console.print("❌ [red]Error:[/red] Could not locate QuanuX repository root.")
        raise typer.Exit(code=1)

    dest_dir = repo_root / "extensions" / "cpp" / ext_name / "vendor"
    console.print(f"🎯 Injection Target: [blue]{dest_dir}[/blue]")

    if dry_run:
        console.print("⚠️  [yellow]DRY RUN[/yellow]: No files will be moved.")
        return

    if dest_dir.exists():
        console.print("🧹 Cleaning previous injection...")
        shutil.rmtree(dest_dir)

    dest_dir.mkdir(parents=True, exist_ok=True)

    try:
        # Copy include
        shutil.copytree(sdk_path / "include", dest_dir / "include")
        # Copy lib
        shutil.copytree(sdk_path / "lib", dest_dir / "lib")
        # Copy doc
        if (sdk_path / "doc").exists():
            shutil.copytree(sdk_path / "doc", dest_dir / "doc")
        
        console.print(f"🚀 [bold green]Success![/bold green] Injected {ext_name} SDK.")
        console.print("🔒 Security Note: 'vendor' directory is git-ignored.")
        
    except Exception as e:
        console.print(f"❌ [red]Injection Failed:[/red] {str(e)}")
        raise typer.Exit(code=1)
