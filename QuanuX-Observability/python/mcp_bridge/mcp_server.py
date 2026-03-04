import os
import requests
import telemetry_compiler
from mcp.server.fastmcp import FastMCP

# Initialize MCP Bridge Server
mcp = FastMCP("QuanuX-Observability-Bridge")

# Nexus Supergraph Endpoint
NEXUS_URL = "http://10.10.10.9:8080/v1/graphql"

@mcp.tool()
def read_historical_telemetry(limit: int = 100) -> str:
    """
    Fetches raw historical telemetry from the Hasura GraphQL Supergraph
    and translates it to Markdown instantly via Cython.
    """
    # 1. Dynamic Security Injection (Zero Plaintext Rule)
    hasura_secret = os.environ.get("HASURA_GRAPHQL_ADMIN_SECRET")
    if not hasura_secret:
        return "[FATAL] HASURA_GRAPHQL_ADMIN_SECRET environment variable not set in MCP process."

    headers = {
        "Content-Type": "application/json",
        "x-hasura-admin-secret": hasura_secret
    }

    query = f"""
    query FetchTelemetry {{
      quanux_telemetry_live(limit: {limit}) {{
        timestamp
        cpu_usage
        memory_usage
        latency_ns
      }}
    }}
    """

    try:
        # 2. The GraphQL Fetch Phase
        response = requests.post(NEXUS_URL, json={'query': query}, headers=headers)
        response.raise_for_status()
        data = response.json()
        
        # Strip boilerplate payload wrappers
        rows = data.get("data", {}).get("quanux_telemetry_live", [])
        if not rows:
            return "No telemetry data found for QuanuX."
            
        columns = ["timestamp", "cpu_usage", "memory_usage", "latency_ns"]
        
        # 3. The Cython Translation Handoff (C-level execution)
        # Bypassing pure Python loops entirely for formatting.
        markdown_output = telemetry_compiler.json_to_markdown(rows, columns)
        
        return markdown_output
        
    except requests.exceptions.RequestException as e:
        return f"[FATAL] Nexus API Communication Failure: {str(e)}"

if __name__ == "__main__":
    mcp.run_stdio_async()
