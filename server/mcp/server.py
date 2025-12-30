
import asyncio
import os
import subprocess
from pathlib import Path
from typing import List, Optional
from mcp.server.fastmcp import FastMCP

# Initialize FastMCP server
# "QuanuX Platform" is the name of the service
mcp = FastMCP("QuanuX Platform")

BASE_DIR = Path(os.getcwd())

@mcp.tool(name="repo.search")
async def repo_search(pattern: str, search_path: str = ".") -> str:
    """Search for files matching a glob pattern."""
    safe_path = (BASE_DIR / search_path).resolve()
    if not str(safe_path).startswith(str(BASE_DIR)):
        return "Error: Path must be within the repository."
    
    matches = []
    try:
        for file in safe_path.rglob(pattern):
             if ".git" in str(file) or "node_modules" in str(file):
                 continue
             matches.append(str(file.relative_to(BASE_DIR)))
             if len(matches) > 100:
                 matches.append("... truncated ...")
                 break
    except Exception as e:
        return f"Error searching: {e}"
    return "\n".join(matches) if matches else "No matches found."

@mcp.tool(name="repo.open")
async def repo_open(path: str) -> str:
    """Read the contents of a file."""
    safe_path = (BASE_DIR / path).resolve()
    if not str(safe_path).startswith(str(BASE_DIR)):
        return "Error: Path must be within the repository."
    
    if not safe_path.exists() or not safe_path.is_file():
        return f"Error: File not found: {path}"
        
    try:
        return safe_path.read_text()
    except Exception as e:
        return f"Error reading file: {e}"

@mcp.tool(name="build.web")
async def build_web() -> str:
    """Trigger a build of the Web Client (Vite)."""
    cwd = BASE_DIR / "client" / "web"
    try:
        proc = await asyncio.create_subprocess_exec(
            "npm", "run", "build",
            cwd=str(cwd),
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
        stdout, stderr = await proc.communicate()
        status = "Success" if proc.returncode == 0 else "Failed"
        return f"Web Build {status}:\n{stdout.decode()}\n{stderr.decode()}"
    except Exception as e:
        return f"Error triggering build: {e}"

@mcp.tool(name="build.desktop")
async def build_desktop() -> str:
    """Trigger a build of the Desktop Application (Tauri)."""
    cwd = BASE_DIR / "client" / "desktop" / "tauri-app"
    try:
        proc = await asyncio.create_subprocess_exec(
            "npm", "run", "tauri:build",
            cwd=str(cwd),
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
        stdout, stderr = await proc.communicate()
        status = "Success" if proc.returncode == 0 else "Failed"
        return f"Desktop Build {status}:\n{stdout.decode()}\n{stderr.decode()}"
    except Exception as e:
        return f"Error triggering build: {e}"

@mcp.tool(name="test.server")
async def test_server(path: str = "server/tests") -> str:
    """Run server-side tests using pytest."""
    try:
        proc = await asyncio.create_subprocess_exec(
            "pytest", path,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE
        )
        stdout, stderr = await proc.communicate()
        status = "PASSED" if proc.returncode == 0 else "FAILED"
        return f"Tests {status}:\n{stdout.decode()}\n{stderr.decode()}"
    except Exception as e:
        return f"Error running tests: {e}"

@mcp.tool(name="duckdb.query_readonly")
async def duckdb_query(query: str) -> str:
    """Execute a read-only SQL query against DuckDB."""
    # Guard against obvious non-readonly queries
    if any(k in query.lower() for k in ["insert", "update", "delete", "drop", "alter"]):
        return "Error: Only read-only queries are allowed."
        
    try:
        import duckdb
        # Use in-memory connection for now or connect to file if specified
        # Assuming analysis on parquet/csv files in data/
        con = duckdb.connect(database=':memory:')
        # Verify if we should mount data dir? For now, straight query.
        df = con.sql(query).df()
        return df.to_markdown()
    except ImportError:
        return "Error: duckdb package not installed."
    except Exception as e:
        return f"Query Error: {e}"

if __name__ == "__main__":
    mcp.run()
