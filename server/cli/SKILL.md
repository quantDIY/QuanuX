---
name: quanux-cli
description: "CLI Development (Typer, Rich) and Module Lifecycle Management"
---

# QuanuX CLI (`quanuxctl`)

## Overview
The CLI is built using **Typer** (for command parsing) and **Rich** (for beautiful output).
Source: `server/cli/src/quanuxctl/`

## 1. Adding a New Command Group
To add a new namespace (e.g., `quanuxctl mycommand ...`):

1.  **Create Module**:
    Create `server/cli/src/quanuxctl/commands/mycommand.py`:
    ```python
    import typer
    from rich.console import Console

    app = typer.Typer(help="Description of my command group.")
    console = Console()

    @app.command()
    def hello(name: str):
        """Say hello."""
        console.print(f"[green]Hello {name}[/green]")
    ```

2.  **Register**:
    Edit `server/cli/src/quanuxctl/main.py`:
    ```python
    from .commands import mycommand
    app.add_typer(mycommand.app, name="mycommand", help="Description")
    app.add_typer(mycommand.app, name="m", help="Alias", hidden=True)
    ```

## 2. Module Lifecycle Pattern (Core Protocol)
If you are building a "Core Module" (like Indicators, Backtester, etc.), you **MUST** provide lifecycle management commands in the CLI.

**Standard Commands**:
*   `install`: Scaffolds the directory structure or pulls from source.
*   `remove`: Deletes the module directory (destructive).
*   `check`: Verifies integrity (headers, config).

**Example (`server/cli/src/quanuxctl/commands/module.py`)**:
Use this pattern to allow users to "uninstall" portions of the system they don't use.

## 3. Best Practices
## 4. Developing TUI Applications (Textual)
When building interactive terminal apps (like `dashboard`), follow these patterns:

1.  **Async & Threads**:
    -   Use `@work(exclusive=True)` for long-running or async tasks (like NATS subscriptions).
    -   **NEVER** block the main thread.
    -   **ALWAYS** use `self.call_from_thread(callback, ...)` when updating the UI from a background task (like a NATS callback).

2.  **Screen Architecture**:
    -   Define logic inside `Screen` classes (e.g., `GridScreen`) rather than the main `App` to avoid widget query race conditions.
    -   Connect services (NATS, DB) in `on_mount()`.

3.  **Example Pattern**:
    ```python
    class MyScreen(Screen):
        def on_mount(self):
            self.start_background_task()
    
        @work(exclusive=True)
        async def start_background_task(self):
            # Async work here
            self.call_from_thread(self.update_ui, data)
    ```

