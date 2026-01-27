# QuanuX System Dashboard (TUI)

The **System Dashboard** (`quanuxctl dashboard start`) is a terminal-based mission control interface for the QuanuX grid. It provides real-time visibility into distributed nodes, process health, and resource usage.

## Features

1.  **Grid View**: Auto-discovers active nodes via NATS heartbeats.
2.  **Live Metrics**: Updates CPU, Memory, and Uptime in real-time.
3.  **Log Streaming**: Displays system-wide logs directly in the terminal.

## Architecture

The dashboard is built using **Textual**, a TUI framework for Python.

-   **Entry Point**: `server/cli/src/quanuxctl/commands/dashboard.py`
-   **State Management**: NATS subscriptions update an internal state dictionary (`self.app.nodes`), which triggers UI refreshes on the main thread via `call_from_thread`.
-   **Concurrency**: Uses `asyncio` with Textual's `@work` decorator to handle NATS messages without blocking the UI loop.

## Extensibility Guide

The dashboard is designed to be easily extended. Here is a guide for developers (and AI agents) to add new capabilities.

### 1. Adding New Metrics
To display a new data point (e.g., "Strategy PnL"):

1.  **Backend**: Ensure your node publishes the metric in its heartbeat or telemetry message.
2.  **UI (GridScreen)**:
    -   In `GridScreen.on_mount`: Add a new column: `table.add_column("PnL")`.
    -   In `update_node_table`: Extract the value from `data` and call `table.update_cell(node_id, "PnL", value)`.

### 2. Adding New Views (Screens)
To create a drill-down view (e.g., detailed process list for a node):

1.  **Define Screen**:
    Create a new class inheriting from `Screen`:
    ```python
    class NodeDetailScreen(Screen):
        def compose(self) -> ComposeResult:
            yield Label(f"Details for {self.node_id}")
            yield DataTable(id="proc-table")
    ```

2.  **Register Screen**:
    In `QuanuXDashboard`:
    ```python
    SCREENS = {"grid": GridScreen, "detail": NodeDetailScreen}
    ```

3.  **Navigate**:
    Add an event handler in `GridScreen`:
    ```python
    def on_data_table_row_selected(self, event):
        node_id = event.row_key.value
        self.app.push_screen(NodeDetailScreen(node_id))
    ```

### 3. Adding Interactivity (Commands)
To control nodes (e.g., "Restart Strategy"):

1.  **Add Signal**: Add a Button or Keybinding in the UI.
2.  **Publish Command**:
    Use the NATS connection to send a command message:
    ```python
    await self.app.nc.publish(f"node.{node_id}.ctl", b"restart")
    ```

## Troubleshooting

-   **"Connecting to NATS..." stuck**: Ensure `nats-server` is running and accessible. Default URL is `nats://localhost:4222`.
-   **No Nodes Visible**: Ensure your nodes (e.g., `execution-node`) are running and publishing heartbeats to `node.*.heartbeat`.
