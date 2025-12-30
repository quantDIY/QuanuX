import sys
import os
import shutil
from pathlib import Path

# Add server to path
sys.path.append(os.getcwd())

from server.strategies.builder import StrategyBuilder
from server.security.secrets import KeyringBackend

def run_verification():
    print("🚀 Starting OpenAI Verification...", flush=True)
    
    # 1. Verify Key Access
    kb = KeyringBackend()
    key = kb.get("QUANUX_OPENAI_API_KEY")
    if key:
        masked = key[:8] + "..." + key[-4:]
        print(f"✅ Key retrieval successful: {masked}")
    else:
        print("❌ Key retrieval failed. Did you run setup_secrets.py?")
        return

    # 2. Test Strategy Builder
    builder = StrategyBuilder()
    
    # Simple test inputs
    inputs = {
        "naming": "VerificationStrategy",
        "description": "A simple moving average crossover strategy for verification.",
        "type": "Full Strategy",
        "broker": "None"
    }

    try:
        print("\n🤖 Calling OpenAI (this may take 10-20 seconds)...")
        result = builder.generate_strategy(inputs)
        
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
