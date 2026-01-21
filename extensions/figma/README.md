# Figma MCP Server (Official Integration)

This extension integrates the **official** Figma Developer MCP Server into QuanuX, enabling AI agents to read and analyze your Figma designs.

## Setup

### 1. Generate a Figma Access Token
1.  Log in to Figma.
2.  Go to **Settings** -> **Personal access tokens**.
3.  Click **Generate new token**.
4.  Give it a name (e.g., "QuanuX").
5.  **Scopes**: Ensure you grant `File content: Read` and `Comments: Read` (and `Write` if you want it to post comments).
6.  Copy the token.

### 2. Configure QuanuX
Use `quanuxctl` to securely store the token:

```bash
quanuxctl secrets set figma
# When prompted, paste your Personal Access Token
```

### 3. Verification
The extension will automatically pick up the `FIGMA_ACCESS_TOKEN` injected by QuanuX at runtime.

## Architecture
- **Runtime**: Node.js (v18+)
- **Package**: `figma-developer-mcp` (NPM)
- **Mode**: Sidecar (Runs as a child process of QuanuX Server)
