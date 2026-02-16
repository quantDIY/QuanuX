
import os
import sys
import shutil
import subprocess
from pathlib import Path

ROOT_DIR = Path(os.getcwd())
SCHEMAS_DIR = ROOT_DIR / "meta/schemas/json"
SERVER_OUT_DIR = ROOT_DIR / "server/app/domain/contracts"
CLIENT_OUT_DIR = ROOT_DIR / "client/data/contracts"

def check_tools():
    """Verify required tools are available."""
    # Check pnpm
    if not shutil.which("pnpm"):
        print("❌ Error: 'pnpm' not found. Please install pnpm.")
        sys.exit(1)
    
    # Check datamodel-code-generator
    try:
        import datamodel_code_generator
    except ImportError:
        print("❌ Error: 'datamodel-code-generator' not found. Please run 'pip install datamodel-code-generator'.")
        sys.exit(1)

def clean_outputs():
    """Remove existing generated files to ensure clean state."""
    # We kept __init__.py and index.ts manually created in previous plans, 
    # but strictly speaking regeneration should overwrite or we should control it.
    # For now, let's just ensure directories exist.
    SERVER_OUT_DIR.mkdir(parents=True, exist_ok=True)
    CLIENT_OUT_DIR.mkdir(parents=True, exist_ok=True)

def generate_python_models():
    """
    Generate Pydantic models for the Server.
    We generate a single file 'trading.py' for simplicity if schemas are combined,
    or iterate. 
    Current plan: Iterate schemas or use directory input mode.
    """
    print("🐍 Generating Python Models (Pydantic)...")
    
    # Using 'datamodel-code-generator' CLI wrapper via subprocess for simplicity with arguments
    # Input: meta/schemas/json
    # Output: server/app/domain/contracts
    # We want to treat the directory recursively.
    
    # Preserve __init__.py if it exists (manual exports)
    init_file = SERVER_OUT_DIR / "__init__.py"
    init_content = None
    if init_file.exists():
        print(f"ℹ️  Preserving manual {init_file.name}...")
        init_content = init_file.read_text()

    cmd = [
        sys.executable, "-m", "datamodel_code_generator",
        "--input", str(SCHEMAS_DIR),
        "--input-file-type", "jsonschema",
        "--output", str(SERVER_OUT_DIR),
        "--output-model-type", "pydantic_v2.BaseModel",
        "--use-schema-description",
        "--use-field-description",
        "--disable-timestamp"
    ]
    
    # Note: If we want separate files per schema file, we'd iterate.
    # But usually a single models.py is easier to import from for "contracts".
    # Let's try directory mode first.
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    # Restore __init__.py
    if init_content:
        init_file.write_text(init_content)
        print(f"✅ Restored manual {init_file.name}")

    if result.returncode == 0:
        print(f"✅ Python models generated at: {SERVER_OUT_DIR / 'models.py'}")
    else:
        print(f"❌ Python generation failed:\n{result.stderr}")
        sys.exit(1)

def generate_ts_interfaces():
    """
    Generate TypeScript interfaces for the Client.
    Using 'json2ts' from 'json-schema-to-typescript'.
    """
    print("📘 Generating TypeScript Interfaces...")
    
    # We iterate over JSON files and generate corresponding TS files
    # or a single contracts.ts
    
    # Let's generate a single 'models.ts' to match python 'models.py'
    # We need to find all json files
    schema_files = list(SCHEMAS_DIR.rglob("*.json"))
    
    if not schema_files:
        print("⚠️ No schema files found.")
        return

    output_file = CLIENT_OUT_DIR / "models.ts"
    
    # json2ts can take a file or glob.
    # To combine them, it's tricky with CLI.
    # Easier loop: generate individual files?
    # Or generate one big file? 
    # Let's try generating one file per schema file for TS to avoid name collisions if not namespaced.
    
    for schema_file in schema_files:
        rel_path = schema_file.relative_to(SCHEMAS_DIR)
        out_path = CLIENT_OUT_DIR / rel_path.with_suffix(".ts")
        
        # Ensure parent dir
        out_path.parent.mkdir(parents=True, exist_ok=True)
        
        cmd = [
            "pnpm", "exec", "json2ts",
            "-i", str(schema_file),
            "-o", str(out_path),
            "--style.singleQuote"
        ]
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            print(f"❌ TS generation failed for {schema_file.name}:\n{result.stderr}")
        else:
            print(f"   - Generated {out_path.name}")

    print(f"✅ TypeScript interfaces generated in: {CLIENT_OUT_DIR}")

def main():
    print("⚙️  QuanuX Schema Generator")
    print("==========================")
    check_tools()
    clean_outputs()
    generate_python_models()
    generate_ts_interfaces()
    print("\n✨ Done.")

if __name__ == "__main__":
    main()
