import sys
import os
import shutil
from pathlib import Path

# Add server to path
sys.path.append(os.getcwd())

print("DEBUG: Importing StrategyBuilder...", flush=True)
from server.strategies.builder import StrategyBuilder
print("DEBUG: Importing KeyringBackend...", flush=True)
from server.security.secrets import KeyringBackend
print("DEBUG: Imports complete.", flush=True)

def run_verification():
    print("🚀 Starting Gemini Verification...", flush=True)
    
    # 1. Verify Key Access
    kb = KeyringBackend()
    key = kb.get("QUANUX_GEMINI_API_KEY")
    if key:
        # Show partial key if available
        print(f"✅ Key retrieval successful: ******** (masked)")
    else:
        print("❌ Key retrieval failed. Please run 'python3 server/cli/main.py secrets setup'")
        return

    # 2. Test Strategy Builder
    builder = StrategyBuilder()
    
    inputs = {
        "naming": "GeminiStrategy",
        "description": "A simple mean reversion strategy.",
        "type": "Full Strategy",
        "broker": "None"
    }

    try:
        print("\n🤖 Calling Google Gemini (may take 10-20 seconds)...", flush=True)
        # Pass provider="gemini"
        result = builder.generate_strategy(inputs, provider="gemini")
        
        path = result.get('path')
        if path and Path(path).exists():
            print(f"✅ Strategy generated successfully at: {path}")
            
            # List generated files
            print("   Files created:")
            for f in Path(path).glob("**/*"):
                if f.is_file():
                    print(f"   - {f.name} ({f.stat().st_size} bytes)")
                    
            # Cleanup
            print("\n🧹 Cleaning up test artifacts...")
            shutil.rmtree(path)
            print("✅ Cleanup complete.")
            
        else:
            print("❌ Generation failed: Output path not found.")
            
    except Exception as e:
        print(f"❌ Verification failed: {e}")

if __name__ == "__main__":
    run_verification()
