#!/usr/bin/env python3
import os
import sys
import shutil
from pathlib import Path

def setup_env():
    # Define paths
    project_root = Path(__file__).resolve().parent.parent
    sdk_etc_path = project_root / "extensions" / "cpp" / "rithmic" / "sdk" / "etc"
    cert_file_name = "rithmic_ssl_cert_auth_params"
    
    target_ssl_dir = project_root / "ssl"
    target_cert_path = target_ssl_dir / cert_file_name
    
    # Check source
    source_cert_path = sdk_etc_path / cert_file_name
    if not source_cert_path.exists():
        print(f"ERROR: Source certificate file not found at: {source_cert_path}")
        print("Please ensure the Rithmic SDK is correctly installed.")
        sys.exit(1)
        
    # check runtime directory (where we run python from, usually project root)
    # The C++ SDK looks for ./ssl/rithmic_ssl_cert_auth_params relative to CWD
    
    print(f"Setting up Rithmic SSL environment...")
    print(f"Source: {source_cert_path}")
    print(f"Target: {target_cert_path}")
    
    try:
        if not target_ssl_dir.exists():
            target_ssl_dir.mkdir(parents=True, exist_ok=True)
            print(f"Created directory: {target_ssl_dir}")
            
        if target_cert_path.exists() or target_cert_path.is_symlink():
            target_cert_path.unlink()
            print(f"Removed existing link/file at target.")
            
        # Create symlink
        os.symlink(source_cert_path, target_cert_path)
        print(f"SUCCESS: Symlinked certificate to {target_cert_path}")
        
    except Exception as e:
        print(f"FAILURE: Could not setup environment: {e}")
        sys.exit(1)

if __name__ == "__main__":
    setup_env()
