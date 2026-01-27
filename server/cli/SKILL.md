---
name: quanux-cli
description: Reference for using the QuanuX Control CLI (quanuxctl).
---

# QuanuX Control CLI (`quanuxctl`)

The `quanuxctl` tool is the central management interface for the QuanuX platform.

## Key Capabilities

### 1. Extension Management
- `quanuxctl ext list`: View all installed extensions and their status.
- `quanuxctl ext start/stop <name>`: Lifecycle management.
- `quanuxctl topstepx`: Dedicated namespace for TopstepX management.

### 2. Secret Management
- `quanuxctl secrets set <key> <value>`: Store secrets in system Keyring.
- `quanuxctl secrets get <key>`: Retrieve secrets (for debugging).
- `quanuxctl secrets setup`: Interactive wizard.

### 3. TopstepX Specifics
- `quanuxctl topstepx install`: Setup dependencies.
- `quanuxctl topstepx env`: Check configuration.
- `quanuxctl topstepx user/password/apikey`: Credential rotation.
- `quanuxctl topstepx user-hub/market-hub`: SignalR endpoint configuration.

## Development
The CLI is built with **Typer**.
- **Source**: `server/cli/src/quanuxctl`
- **Commands**: `server/cli/src/quanuxctl/commands/`
- **Adding Commands**: Create a new module in `commands/` and register it in `main.py`.

## Running Locally
To run the CLI from source without installing:
```bash
python -m server.cli.src.quanuxctl.main <command>
```
