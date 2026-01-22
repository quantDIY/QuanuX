---
name: figma-integration
description: Official Figma Developer MCP integration for QuanuX. Allows agents to inspect designs, extract variables, and generate code from Figma files.
metadata:
  version: 1.0.0
  author: Figma / QuanuX
  priority: 50
---

# Figma Integration (MCP)

This skill provides access to the **Figma Developer MCP Server**, allowing you to read design data, inspect layers, and extract code snippets directly from Figma files.

## Protocol Enforcement (CRITICAL)

**When using this tool to generate code, you MUST adhere to the following QuanuX Standards:**

1.  **Read the Rules**: Before generating any code, you MUST read `client/skills/react-frontend-standards/SKILL.md`.
2.  **Understand Data Layer**: Read `docs/ARCHITECTURE_DATA_LAYER.md` to map data needs to the correct transport (API vs Stream).
3.  **Server-Side Logic**: Figma designs often imply client-side logic. **REJECT THIS**. All business logic must reside on the server.
4.  **No Mock Data**: Do not generate components with hardcoded mock data arrays. Request backend models instead.
5.  **Tech Stack**: Output must use **Tailwind CSS** and **Shadcn UI** (Radix) components.

## Prerequisites

-   **Secret**: `QUANUX_figma` must be set via `quanuxctl secrets set figma`.
-   **Status**: Extension must be running (`quanuxctl ext start figma`).

## Tools

The following tools are exposed by the underlying `figma-developer-mcp` server:

### `get_design_context`
Retrieves detailed design context for a specific layer or selection. Use this to understand the structure and properties of a component.

### `get_code`
Extracts code (e.g. React/Tailwind) from a Figma selection or URL.
- **Input**: Figma URL or Selection ID.
- **Output**: Code map and implementation details.

### `get_variable_defs`
Returns the design tokens (variables) and styles used within a selection. Essential for adhering to the Design System.

### `get_metadata`
Returns a simplified XML representation of the selection (IDs, names, types, positions). Light-weight structural analysis.

### `get_screenshot`
Captures a visual screenshot of the selection. useful for multi-modal analysis.

### `create_design_system_rules`
Generates a rule file (context) to help translate designs into code that matches the project's existing design system.

### `whoami`
Returns the identity of the authenticated Figma user.

## Usage Example

To get code for a specific button:
1.  Use `get_metadata` to find the node ID of the button (or use a direct URL).
2.  Call `get_code` with that ID.
3.  Use `get_variable_defs` to ensure you are using the correct color tokens.
