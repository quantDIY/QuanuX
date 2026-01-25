# QuanuX Control CLI (`quanuxctl`)

A unified developer CLI for managing the QuanuX organism.

## Structure

*   **Framework**: [Typer](https://typer.tiangolo.com/)
*   **UI/Output**: [Rich](https://rich.readthedocs.io/)
*   **Entry Point**: `server/cli/src/quanuxctl/main.py`

## Common Commands

*   `quanuxctl secrets set <KEY>`: securely store API keys.
*   `quanuxctl bridge start`: Start the SignalR/Rithmic bridge.
*   `quanuxctl skills list`: View available AI skills.
*   `quanuxctl module remove <name>`: Uninstall a core subsystem.

## Extending

See `SKILL.md` for AI agent instructions on adding new command groups.

1.  Add a submodule in `src/quanuxctl/commands/`.
2.  Register it in `src/quanuxctl/main.py`.
