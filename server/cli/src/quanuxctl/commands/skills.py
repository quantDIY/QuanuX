import typer
from rich.console import Console
from rich.table import Table
from rich.markdown import Markdown
from server.mcp.skills import SkillLoader

app = typer.Typer()
console = Console()

# specific path needs to be handled carefully depending on where quanuxctl is run from
# Assuming running from root as per usual dev workflow, or we need to find root
skill_loader = SkillLoader(["server/skills", "client/skills"])

@app.command("list")
def list_skills():
    """List all available agent skills."""
    skills = skill_loader.list_skills()
    
    if not skills:
        console.print("[yellow]No skills found.[/yellow]")
        return

    table = Table(title="Available Agent Skills")
    table.add_column("Name", style="cyan", no_wrap=True)
    table.add_column("Description", style="magenta")
    table.add_column("Path", style="dim")

    for skill in skills:
        table.add_row(skill.name, skill.description, skill.tool_authority_path)

    console.print(table)

@app.command("read")
def read_skill(skill_name: str):
    """Read the content of a specific skill."""
    content = skill_loader.read_skill(skill_name)
    
    if content.startswith("Error:"):
        console.print(f"[red]{content}[/red]")
    else:
        md = Markdown(content)
        console.print(md)
