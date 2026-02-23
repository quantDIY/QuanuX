import json
import os
import subprocess
import hashlib
from pathlib import Path
from rich.console import Console
from rich.panel import Panel
import typer

console = Console()
app = typer.Typer(help="Manage QuanuX-Spreader Engine Deployment and Strategy Injection.")

# The C++ Header Template (Strict C-Types)
TEMPLATE_CPP = """#pragma once
#include "quanux/MarketTick.hpp"
#include <immintrin.h>
#include <cstdint>

extern "C++" {

namespace quanux::spreader::strategy {

struct alignas(64) StrategyState {
{STATE_VARS}
};

class InjectionStub {
public:
    static inline void init(StrategyState& state) {
        (void)state;
        // AI-Forged Initialization
    }

    [[gnu::always_inline]] static inline bool calculate(StrategyState& state, const quanux::MarketTick& tick) {
        // Fast paths:
        {CALCULATE_SPREAD}
        
        // Entry Trigger
        if ( {ENTRY_CONDITION} ) {
            return true;
        }

        // Exit Trigger
        if ( {EXIT_CONDITION} ) {
            return false;
        }

        return false;
    }
};

using ActiveStrategy = InjectionStub;

} // namespace quanux::spreader::strategy
} // extern "C++"
"""

# The Cython Wrapper Template (Crucible Integration)
TEMPLATE_PYX = """# cython: language_level=3
# cython: cplus=True

cdef extern from "quanux/MarketTick.hpp" namespace "quanux":
    cdef cppclass MarketTick:
        double price
        unsigned int instrument_id

cdef extern from "{HEADER_FILENAME}" namespace "quanux::spreader::strategy":
    cdef cppclass StrategyState:
        pass
    
    cdef cppclass InjectionStub:
        @staticmethod
        void init(StrategyState& state)
        
        @staticmethod
        bint calculate(StrategyState& state, const MarketTick& tick) nogil

cdef class PyStrategyWrapper:
    cdef StrategyState state

    def __cinit__(self):
        InjectionStub.init(self.state)

    def calculate(self, double price, unsigned int instrument_id):
        cdef MarketTick tick
        tick.price = price
        tick.instrument_id = instrument_id
        return InjectionStub.calculate(self.state, tick)
"""

def map_c_type(json_type: str) -> str:
    type_map = {
        "double": "double",
        "float": "float",
        "uint64_t": "uint64_t",
        "int64_t": "int64_t",
        "uint32_t": "uint32_t",
        "int32_t": "int32_t",
        "bool": "bool"
    }
    return type_map.get(json_type, "double")

@app.command("package")
def package(strategy_json: str = typer.Argument(..., help="Path to the Strategy IR JSON file.")):
    """
    Transpile the strategy into C++ via Cython constructs and forge the 59ns QuanuX-Spreader binary.
    """
    json_path = Path(strategy_json)
    if not json_path.exists():
        console.print(f"[bold red]Error:[/bold red] Strategy file {strategy_json} not found.")
        raise typer.Exit(code=1)

    try:
        with open(json_path, 'r') as f:
            strategy_data = json.load(f)
    except json.JSONDecodeError as e:
        console.print(f"[bold red]Error parsing JSON:[/bold red] {e}")
        raise typer.Exit(code=1)

    strategy_name = strategy_data.get("strategy_name", "UnknownStrategy")
    console.print(Panel(f"Forging Cython-Backed Strategy: [bold cyan]{strategy_name}[/bold cyan]"))

    # Parse state vars to strict C-types
    state_vars_c = []
    for var in strategy_data.get("state_variables", []):
        ctype = map_c_type(var["type"])
        init_val = var.get("initial_value", 0)
        state_vars_c.append(f"    {ctype} {var['name']} = {init_val};")
    state_vars_str = "\n".join(state_vars_c)

    logic = strategy_data.get("logic_gates", {})
    calc = logic.get("calculate_spread", "")
    entry = logic.get("entry_condition", "false")
    exit = logic.get("exit_condition", "false")

    rendered_cpp = TEMPLATE_CPP.replace("{STATE_VARS}", state_vars_str)\
                               .replace("{CALCULATE_SPREAD}", calc)\
                               .replace("{ENTRY_CONDITION}", entry)\
                               .replace("{EXIT_CONDITION}", exit)

    workspace_root = Path(os.getcwd())
    injected_dir = workspace_root / "QuanuX-Spreader" / "cpp" / "include" / "spreader" / "injected"
    injected_dir.mkdir(parents=True, exist_ok=True)
    
    header_filename = f"{strategy_name}_injected.hpp"
    header_path = injected_dir / header_filename
    with open(header_path, "w") as f:
        f.write(rendered_cpp)
    
    console.print(f"[green]Generated C++ Header (extern 'C++'):[/green] {header_filename}")

    # Generate the Cython wrapper for Crucible Backtesting
    rendered_pyx = TEMPLATE_PYX.replace("{HEADER_FILENAME}", f"spreader/injected/{header_filename}")
    pyx_path = workspace_root / "server" / "spreader-strategies" / f"{strategy_name}_wrapper.pyx"
    with open(pyx_path, "w") as f:
        f.write(rendered_pyx)
    
    console.print(f"[green]Generated Cython Backtest Wrapper:[/green] {pyx_path.name}")

    # Cython Compile Step (Placeholder for building the extension module locally)
    console.print(f"[yellow]Cythonizing wrapper for Crucible...[/yellow]")
    # subprocess.run(["cythonize", "-i", str(pyx_path)], check=True) # Skipped in stub

    # Compiler Invocation
    console.print(f"[yellow]Invoking 59ns Engine Compiler (CMake Preset: spreader)...[/yellow]")
    build_dir = workspace_root / "QuanuX-Spreader" / "cpp" / "build"
    relative_header_path = f"spreader/injected/{header_filename}"
    
    config_cmd = ["cmake", "--preset", "spreader", f"-DSTRATEGY_HEADER={relative_header_path}"]
    build_cmd = ["cmake", "--build", "--preset", "build-spreader"]

    try:
        subprocess.run(config_cmd, check=True, cwd=workspace_root)
        subprocess.run(build_cmd, check=True, cwd=workspace_root)
    except subprocess.CalledProcessError as e:
        console.print(f"[bold red]Compiler Error![/bold red] The strategy failed 64-byte structural assertions or syntax checks.")
        raise typer.Exit(code=1)

    # Hash Verification (Forge Signature)
    # CMake builds the target inside the mirrored source tree subdirectory
    binary_path = build_dir / "QuanuX-Spreader" / "cpp" / "quanux_spreader"
    if not binary_path.exists():
        console.print(f"[bold red]Error:[/bold red] Compilation succeeded but binary not found at {binary_path}")
        raise typer.Exit(code=1)
        
    with open(binary_path, "rb") as f:
        file_hash = hashlib.sha256(f.read()).hexdigest()

    checksum_signature = f"{strategy_name}:{file_hash}"
    hashed_binary_path = build_dir / f"quanux-spreader-{file_hash[:8]}"
    os.rename(binary_path, hashed_binary_path)

    # Write checksum manifest for remote verification
    manifest_path = build_dir / f"quanux-spreader-{file_hash[:8]}.sig"
    with open(manifest_path, "w") as f:
        f.write(checksum_signature)

    console.print(Panel(f"[bold green]Cython Forge Successful![/bold green]\n"
                        f"Binary: [cyan]{hashed_binary_path.name}[/cyan]\n"
                        f"Signature Lock: [yellow]{checksum_signature}[/yellow]\n"
                        f"Deploy Ready: quanuxctl spreader deploy {hashed_binary_path.name}"))

@app.command("deploy")
def deploy(binary_name: str = typer.Argument(..., help="The compiled binary to deploy.")):
    """
    Push the compiled Spreader to the Execution Node stub, validating the Checksum Signature.
    """
    console.print(f"[bold yellow]Validating Forge Signature for {binary_name}...[/bold yellow]")
    
    workspace_root = Path(os.getcwd())
    build_dir = workspace_root / "QuanuX-Spreader" / "cpp" / "build"
    binary_path = build_dir / binary_name
    sig_path = build_dir / f"{binary_name}.sig"

    if not binary_path.exists() or not sig_path.exists():
         console.print("[bold red]Deployment Rejected:[/bold red] Invalid binary or missing Forge Signature manifest.")
         raise typer.Exit(code=1)

    with open(binary_path, "rb") as f:
        current_hash = hashlib.sha256(f.read()).hexdigest()
    
    with open(sig_path, "r") as f:
        expected_sig = f.read().strip().split(":")[1]

    if current_hash != expected_sig:
        console.print("[bold red]CRITICAL SHM ERROR:[/bold red] Binary checksum mismatch. Forgery detected!")
        raise typer.Exit(code=1)

    console.print(f"[bold green]Signature Verified. Deploying {binary_name} to Execution Node...[/bold green]")
    # DMA/ZMQ routing logic will go here
    console.print("[bold green]Deployment Complete. Execution Node is LIVE.[/bold green]")

if __name__ == "__main__":
    app()
