import sys
import os
import glob
from pathlib import Path

# Add the current directory to sys.path so we can import the shared object
# This is necessary because the .so file will have a platform-specific name
current_dir = Path(__file__).parent.resolve()
sys.path.append(str(current_dir))

try:
    # Attempt to find and import the compiled module
    # It will be named something like rithmic_py.cpython-311-darwin.so
    # But we import it as "rithmic_py"
    import rithmic_py # type: ignore
    
    # Re-export key classes/functions for cleaner access
    REngine = rithmic_py.REngine
    REngineParams = rithmic_py.REngineParams
    LoginParams = rithmic_py.LoginParams
    RCallbacks = rithmic_py.RCallbacks
    AlertInfo = rithmic_py.AlertInfo
    
    # Reports & Data
    LineInfo = rithmic_py.LineInfo
    OrderFillReport = rithmic_py.OrderFillReport
    OrderFailureReport = rithmic_py.OrderFailureReport
    BidInfo = rithmic_py.BidInfo
    AskInfo = rithmic_py.AskInfo
    TradeInfo = rithmic_py.TradeInfo
    
    # Order Params
    LimitOrderParams = rithmic_py.LimitOrderParams
    ModifyLimitOrderParams = rithmic_py.ModifyLimitOrderParams
    
    version = rithmic_py.version
    
except ImportError as e:
    print(f"Warning: Could not import compiled Rithmic C++ module: {e}")
    # Fallback or dummy implementation for development/linting could go here
    rithmic_py = None
