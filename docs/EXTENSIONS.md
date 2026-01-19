# QuanuX Extensions Guide

**QuanuX Extension Protocol (QXP)** allows you to "bolt-on" any external capability to the QuanuX Core without modifying the main codebase.

## 1. How it Works
Extensions are standalone programs (usually written in Go) that run alongside QuanuX. They communicate via HTTP/WebSocket and are authenticated using local keys.

## 2. Managing Extensions
Use `quanuxctl` or the **Settings** page in the Web/Desktop app to manage keys.

### Directory Structure
All extensions live in `/extensions`. See specific guides for details:

-   **[Figma Integration](./FIGMA_INTEGRATION.md)**: MCP Server for UI generation.
-   **[Sierra Chart](./SIERRA_CHART_INTEGRATION.md)**: DTC Client for Desktop Trading.
-   **[Rithmic](./RITHMIC_INTEGRATION.md)**: High-performance Futures Data.
-   **[Brokerages](./BROKERAGE_INTEGRATION.md)**: IBKR, Tradovate, SignalR.
-   **[MCP Integration](./MCP_INTEGRATION.md)**: General MCP Tooling.

```text
/extensions
  /n8n              # Go bridge for n8n
  /sierra-chart     # DTC Client
  /figma            # Figma MCP Server
  /rithmic          # Rithmic Market Data Pump
  /signalr          # Generic SignalR Connector
  /ibkr             # Interactive Brokers TWS Client
  /tradovate        # Tradovate WebSocket Client
  /tradingview-udf  # UDF Data Feed for Charts
```

## 3. Configuration
Extensions are configured via **Environment Variables** (Secrets).
See `extensions/SKILL.md` for the architectural standards.

## 4. Running an Extension
1.  **Generate Key**: Go to Settings -> QuanuX Extensions -> Generate Key.
2.  **Start Extension**:
    ```bash
    cd extensions/<name>
    export QUANUX_BRIDGE_KEY=$(quanuxctl secrets get QUANUX_<NAME>_KEY)
    go run main.go
    ```

## 5. Remote Connectivity
If QuanuX is running remotely (e.g. Cloud) and the Extension is local (e.g. Desktop):
-   Extensions support `QUANUX_<NAME>_HOST`.
-   Use **SSH Reverse Tunnels** (`ssh -R`) to expose your local port to the cloud.
