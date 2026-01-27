import os
from pathlib import Path

def create_spyder_config():
    root = Path.cwd()
    spy_dir = root / ".spyproject" / "config"
    spy_dir.mkdir(parents=True, exist_ok=True)

    # 1. codestyle.ini
    (spy_dir / "codestyle.ini").write_text("""[main]
convention = pep8
""")

    # 2. encoding.ini
    (spy_dir / "encoding.ini").write_text("""[main]
encoding = utf-8
""")

    # 3. workspace.ini (Crucial for python path)
    # Spyder 5+ uses a specific format.
    workspace_content = f"""[workspace]
restore_data_on_startup = True
save_data_on_exit = True
save_history = True
save_non_project_files = False
project_type = 'empty-project-type'
recent_files = []

[main]
version = 0.2.0
"""
    (spy_dir / "workspace.ini").write_text(workspace_content)
    
    print(f"Spyder project configuration created at {spy_dir}")
    print("To use: Open Spyder -> Projects -> Open Project -> Select 'QuanuX' folder")

if __name__ == "__main__":
    create_spyder_config()
