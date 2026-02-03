import pytest
import sys
import os

# Ensure we can import the extension
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))

try:
    from rithmic_ext import PyREngineParams, PyLoginParams, RCallbacksBase, PyREngine
except ImportError:
    pytest.fail("Could not import rithmic_ext. ensure it is built.")

def test_params_instantiation():
    """Verify we can create params objects and set fields."""
    p = PyREngineParams()
    p.app_name = "QuanuX"
    assert p.app_name == "QuanuX"
    
    p.app_version = "1.0.0"
    assert p.app_version == "1.0.0"
    
def test_login_params():
    """Verify login params setters."""
    lp = PyLoginParams()
    # Note: No getters were exposed in rithmic.pyx for LoginParams, only setters.
    # We just verify it doesn't crash.
    lp.set_md_user("user")
    lp.set_md_password("pass")
    
class MockCallbacks(RCallbacksBase):
    def __init__(self):
        self.alerts = []
        
    def alert(self, info):
        self.alerts.append(info)

def test_callback_shim():
    """Verify callback inheritance."""
    cb = MockCallbacks()
    assert isinstance(cb, RCallbacksBase)
    # Cannot easily test C++ trigger without real engine, but this verifies python side.

# Note: Testing PyREngine requires a real RApiPlus library link usually.
# If rithmic.cpp is compiled with stubs or mocks, it might work.
# If it expects dylibs, it might crash on __init__.
# We'll skip PyREngine init for this basic sanity check unless we know env is ready.
