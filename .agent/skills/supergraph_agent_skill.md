---
description: The AI Agent manual for querying the QuanuX superGraph via the FastMCP Cython bridge.
---

# QuanuX superGraph Agent Skill

## 1. Objective
This skill defines the exact protocol for autonomous trading agents to query, observe, and react to the QuanuX matrix telemetry via the `FastMCP` integration. The superGraph maps infrastructure state (latency, system load) and trading events (tick flow) into a unified, zero-latency interface.

## 2. FastMCP Tool Interface
The presentation layer of the QuanuX superGraph is powered by a Cythonized bridge (`telemetry_compiler.pyx`) connected to a Hasura GraphQL endpoint.

When querying the superGraph, agents should use the designated FastMCP tool bounds (e.g., `query_supergraph` or the equivalent MCP endpoint). 

### Expected Inputs (JSON)
Agents must submit queries formatted as standard GraphQL fragments within a JSON payload. Example payload:
```json
{
  "query": "query GetTelemetry { quanux_telemetry_live(limit: 10, order_by: {timestamp: desc}) { timestamp cpu_usage memory_usage latency_ns } }"
}
```

### Expected Outputs (C-Compiled Markdown)
To bypass the Python Global Interpreter Lock (GIL) and prevent JSON parsing overhead in the LLM's context window, the Cython bridge instantly translates the Hasura JSON response into a pristine Markdown table.
You will receive responses strictly formatted as:
```markdown
| timestamp | cpu_usage | memory_usage | latency_ns |
|---|---|---|---|
| 2026-03-05T18:27:47.250 | 45.2 | 60.1 | 1500000 |
```

## 3. Standard Operating Procedure (SOP)
As an autonomous agent, you are required to act programmatically if telemetry deviates from acceptable boundaries or if you encounter structural errors.

### Schema Mismatch SOP
If the FastMCP tool returns a schema mismatch or a missing table error (e.g., `Field 'quanux_telemetry_live' not found`):
1. **Do not hallucinate table names.**
2. Immediately query the Hasura introspection schema or FastMCP diagnostic tool to pull the active GraphQL types.
3. Validate if the `materialize_bridge.py` script needs to be re-run by a human or automation to refresh the DuckDB -> Postgres materialization.
4. Notify the Architect that the hot cache has potentially drifted from the MinIO cold storage.

### Anomaly Response SOP
If the returned Cython Markdown table indicates latency spikes (e.g., `latency_ns` exceeding `5000000` or 5ms):
1. Flag a Tier-1 infrastructure warning.
2. Propose suspending active trading/quoting logic on affected Spreader nodes.
3. Check the NATS JetStream ingestion rates to determine if the buffer across the DigitalOcean VPC is saturated.
