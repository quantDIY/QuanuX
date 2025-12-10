# QuanuX MCP Tools Integration

This directory contains scripts and documentation for integrating Model Context Protocol (MCP) tools into the QuanuX workflow.

## Included Tools

### 1. MCP Agent Mail & Beads
**Repo**: [Dicklesworthstone/mcp_agent_mail](https://github.com/Dicklesworthstone/mcp_agent_mail)

-   **Agent Mail**: "Gmail for coding agents". Allows agents to communicate, coordinate, and maintain audit trails.
-   **Beads**: Dependency-aware task planner.

#### Setup
Run the setup script to install both tools:
```bash
./tools/mcp/setup_agent_mail.sh
```
This will:
-   Install `uv` (if missing).
-   Create a Python virtual environment.
-   Install `mcp_agent_mail` and `bd` (Beads CLI).
-   Start the MCP server on port 8765.

#### Usage
-   **Run Server**: `./scripts/run_server_with_token.sh`
-   **Beads CLI**: `bd --help`

### 2. Figma MCP Server
**Official**: Integrated into Figma Dev Mode.

#### Setup (Official)
1.  Open Figma Desktop App.
2.  Enable **Dev Mode** (toggle in the top right).
3.  Go to **Plugins** -> **Development** -> **MCP Server**.
4.  Copy the connection command/token provided by Figma.
5.  Configure your MCP client (e.g., Claude Desktop, Cursor) with the provided details.

#### Usage
Allows AI agents to:
-   Read design hierarchy and properties.
-   Export assets.
-   Generate code from designs.

## Integration with QuanuX
These tools are designed to work alongside the QuanuX development workflow.
-   Use **Beads** to track tasks (`bd create "Implement Feature X"`).
-   Use **Agent Mail** for inter-agent communication if running multiple autonomous agents.
-   Use **Figma MCP** to pull design tokens and layouts directly into `client/src/components`.
