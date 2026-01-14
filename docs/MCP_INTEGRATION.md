# QuanuX MCP Integration Guide

This document outlines the architecture for the **Model Context Protocol (MCP)** within the QuanuX ecosystem. It serves as the reference for adding new tools, capabilities, and Agent Skills.

## 1. The "Tool Authority" Concept

QuanuX Server acts as the **Tool Authority** for all connected clients (IDE, Web App, Desktop App). Instead of each client implementing its own logic for git operations, build systems, or data analysis, they simply connect to the MCP Server.

-   **Single Source of Truth**: `meta/tools/registry.yaml` defines every available tool.
-   **Implementation**: `server/mcp/server.py` implements the logic.
-   **Discovery**: Clients query the MCP server to find out what they can do.

## 2. Agent Skills Architecture

We adhere to the [Agent Skills Specification](https://agentskills.io/specification) to provide specialized capabilities to AI agents.

### The Workflow: Progressive Disclosure

1.  **Discovery (Context Injection)**
    -   **Action**: Client (e.g., IDE) calls `agent.skills.list`.
    -   **Result**: Returns a lightweight XML summary of available skills.
    -   **Usage**: Client injects this list into the **System Prompt** of the AI Agent.
    -   *Crucial*: The Agent now "knows" these skills exist but hasn't read them yet.

2.  **Activation**
    -   **Action**: The Agent realizes a user task matches a skill description.
    -   **Tool Call**: Agent calls `agent.skills.read(skill_name="pdf-processing")`.
    -   **Result**: Returns the full `SKILL.md` content.
    -   **Usage**: The Agent reads the instructions and learns *how* to perform the task.

3.  **Execution (Robust)**
    -   **Action**: The skill requires running a complex script.
    -   **Tool Call**: Agent calls `agent.skills.run_script(...)`.
    -   **Result**: The server executes the script in the skill's directory and returns output.

## 3. How to Add a New Skill

1.  **Create Directory**: `server/skills/<skill-name>/`
2.  **Create SKILL.md**:
    ```markdown
    ---
    name: <skill-name>
    description: <Short description for discovery>
    ---
    # Instructions
    ...
    ```
3.  **Add Scripts (Optional)**: Place scripts in `server/skills/<skill-name>/scripts/`.

## 4. How to Register a New Core Tool

1.  **Define**: Add entry to `meta/tools/registry.yaml`.
2.  **Implement**: Add Python function to `server/mcp/server.py`.
3.  **Decorate**: Use `@mcp.tool(name="...")`.
