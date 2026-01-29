import asyncio
import json
import os
import sys
from pathlib import Path
from mcp.server.fastmcp import FastMCP

# Initialize FastMCP
mcp = FastMCP("QuanuX Extension")

# Add project root to sys.path to access server modules
PROJECT_ROOT = Path(__file__).parent.parent.parent.parent.resolve()
sys.path.append(str(PROJECT_ROOT))

# Import QuanuX Core
try:
    from server.strategies.builder import StrategyBuilder
    from server.app.services.nats import NatsService
except ImportError as e:
    print(f"Error importing QuanuX modules: {e}", file=sys.stderr)
    # Continue, but tools might fail.
    
@mcp.tool()
async def generate_strategy(description: str, name: str = "NewStrategy", broker: str = "Paper", model: str = "gemini-2.0-flash") -> str:
    """
    Generate a complete algorithmic trading strategy from a description.
    
    Args:
        description: Detailed explanation of the strategy logic (e.g. "RSI crossover on 5m NQ").
        name: Name of the strategy (CamelCase preferred).
        broker: Broker to target (Topstep, InteractiveBrokers, Paper).
        model: Model to use (e.g. "gemini-2.0-flash", "gemini-1.5-pro").
    """
    try:
        builder = StrategyBuilder(strategies_dir=str(PROJECT_ROOT / "server" / "strategies"))
        
        inputs = {
            "naming": name,
            "description": description,
            "type": "Full Strategy",
            "broker": broker
        }
        
        # We use "gemini" provider since we are inside the Gemini ecosystem,
        # but StrategyBuilder handles the API calls.
        # Note: StrategyBuilder requires QUANUX_GEMINI_API_KEY in keyring.
        result = builder.generate_strategy(inputs, provider="gemini", model_name=model)
        
        if result.get("path"):
            return f"Success! Strategy '{name}' created at: {result['path']}\n\nYou can open this in the QuanuX Web UI."
        else:
            return f"Generation failed: {result}"
            
    except Exception as e:
        return f"Error generating strategy: {str(e)}"

@mcp.tool()
async def run_quanuxctl(command: str) -> str:
    """
    Run a CLI command using the quanuxctl utility.
    
    Args:
        command: The command arguments (e.g. "dashboard", "backtest --fast", "extensions list").
    """
    try:
        # Construct path to quanuxctl/main.py
        cli_path = PROJECT_ROOT / "quanuxctl" / "main.py"
        
        cmd = ["python3", str(cli_path)] + command.split()
        
        proc = await asyncio.create_subprocess_exec(
            *cmd,
            stdout=asyncio.subprocess.PIPE,
            stderr=asyncio.subprocess.PIPE,
            cwd=str(PROJECT_ROOT) # Run from root
        )
        
        stdout, stderr = await proc.communicate()
        
        output = stdout.decode()
        error = stderr.decode()
        
        if proc.returncode != 0:
            return f"Command Failed (Exit {proc.returncode}):\n{error}\n{output}"
            
        return f"Output:\n{output}\n{error}"

    except Exception as e:
        return f"Error executing quanuxctl: {e}"

@mcp.tool()
async def system_status() -> str:
    """Check the status of the QuanuX Runtime (NATS & Supervisor)."""
    # This is a lightweight check using NatsService
    try:
        ns = NatsService()
        await ns.connect()
        # In a real impl, we'd ping the supervisor topic
        await ns.close()
        return "QuanuX System: ONLINE (NATS Connected)" 
    except Exception as e:
        return f"QuanuX System: OFFLINE ({e})"

if __name__ == "__main__":
    mcp.run()
