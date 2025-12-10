#!/bin/bash
set -e

echo "Installing MCP Agent Mail and Beads..."

# Use the official one-line installer from the repo
curl -fsSL https://raw.githubusercontent.com/Dicklesworthstone/mcp_agent_mail/main/scripts/install.sh | bash -s -- --yes

echo ""
echo "MCP Agent Mail and Beads installed successfully!"
echo "To run the server: ./scripts/run_server_with_token.sh"
echo "To use Beads: bd --help"
