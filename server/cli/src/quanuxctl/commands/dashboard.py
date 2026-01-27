import typer
import asyncio
import json
import logging
from rich.console import Console
from textual.app import App, ComposeResult
from textual.containers import Container, Horizontal, Vertical
from textual.widgets import Header, Footer, DataTable, Static, Log
from textual.screen import Screen
from textual import work
import nats

app = typer.Typer(no_args_is_help=False)
console = Console()

# --- TUI Application ---

# --- TUI Application ---

class GridScreen(Screen):
    """The main view showing the grid of nodes."""
    
    BINDINGS = [("q", "quit", "Quit")]

    def compose(self) -> ComposeResult:
        yield Header(show_clock=True)
        yield Horizontal(
            Static("Active Nodes", classes="header-box"),
            Static("Global CPU: --%", id="global-cpu", classes="header-box"),
            classes="dashboard-header"
        )
        yield DataTable(id="node-table")
        yield Log(id="sys-log", classes="system-log")
        yield Footer()

    def on_mount(self) -> None:
        table = self.query_one(DataTable)
        table.add_columns("Node ID", "Status", "Role", "Uptime", "CPU", "Mem")
        table.cursor_type = "row"
        self.connect_nats()

    @work(exclusive=True)
    async def connect_nats(self):
        log = self.query_one(Log)
        try:
            log.write_line(f"Connecting to NATS: {self.app.nats_url}...")
            self.app.nc = await nats.connect(self.app.nats_url)
            log.write_line("Connected!")
            
            # Subscribe to Heartbeats
            await self.app.nc.subscribe("node.*.heartbeat", cb=self.on_heartbeat)
            log.write_line("Subscribed to node.*.heartbeat")
            
        except Exception as e:
            log.write_line(f"NATS Connection Failed: {e}")

    async def on_heartbeat(self, msg):
        try:
            data = json.loads(msg.data.decode())
            node_id = data.get("id", "unknown")
            # Update internal state (could be more complex, verifying signature etc)
            self.app.nodes[node_id] = data
            
            # Update UI (Must be scheduled on main thread)
            self.call_from_thread(self.update_node_table, node_id, data)
        except:
            pass

    def update_node_table(self, node_id: str, data: dict):
        table = self.query_one(DataTable)
        status = "ONLINE" # If we got a heartbeat
        role = data.get("role", "execution")
        uptime = str(data.get("uptime", 0))
        cpu = f"{data.get('cpu_usage', 0):.1f}%"
        mem = f"{data.get('mem_usage', 0):.1f}%"
        
        # Check if row exists
        try:
            # key is node_id
            table.update_cell(node_id, "Status", status)
            table.update_cell(node_id, "Uptime", uptime)
            table.update_cell(node_id, "CPU", cpu)
            table.update_cell(node_id, "Mem", mem)
        except:
            # Add new row
            table.add_row(node_id, status, role, uptime, cpu, mem, key=node_id)


class QuanuXDashboard(App):
    CSS = """
    .dashboard-header {
        height: 3;
        margin-bottom: 1;
    }
    .header-box {
        border: solid green;
        width: 1fr;
        content-align: center middle;
    }
    .system-log {
        height: 10;
        border-top: solid gray;
    }
    """
    
    SCREENS = {"grid": GridScreen}
    
    def __init__(self, nats_url: str):
        super().__init__()
        self.nats_url = nats_url
        self.nc = None
        self.nodes = {} # node_id -> info

    def on_mount(self) -> None:
        self.push_screen("grid")


# --- CLI Command ---

@app.command()
def start(
    url: str = typer.Option("nats://localhost:4222", help="NATS Hub URL"),
):
    """
    Launch the QuanuX System Dashboard (TUI).
    """
    dashboard = QuanuXDashboard(nats_url=url)
    dashboard.run()

if __name__ == "__main__":
    app()
