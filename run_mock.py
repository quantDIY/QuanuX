
import sys
import os
import logging

# Ensure extensions/python/wrappers is in path
sys.path.append('extensions/python/wrappers')

from server.bridges.rithmic_bridge import RithmicBridge

logging.basicConfig(level=logging.INFO)

def main():
    bridge = RithmicBridge("DEMO", "DEMO", "Rithmic Paper Trading")
    try:
        # We expect login to fail or block if params are invalid, but we want to test instantiation
        print("Bridge instantiated successfully.")
        print(f"Wrapper Version: {bridge.engine_params.sAppName}") 
        # Actually version is global, but we can check if object created
        
        # Don't actually connect to avoid hanging or errors if no creds
        # bridge.connect() 
        
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()
