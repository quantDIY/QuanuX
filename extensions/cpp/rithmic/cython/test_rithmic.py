import rithmic_ext
import sys

print("Successfully imported rithmic_ext")

class MyCallbacks(rithmic_ext.RCallbacksBase):
    def alert(self, info):
        print(f"Alert: {info}")

    def line_update(self, info):
        print(f"Line Update: {info}")

def test_instantiation():
    print("Testing instantiation...")
    try:
        # Params
        params = rithmic_ext.PyREngineParams()
        params.app_name = "QuanuX"
        params.app_version = "0.1.0"
        params.log_file_path = "rithmic.log"
        print(f"Params created: {params.app_name} v{params.app_version}")

        # Engine
        # Note: REngine constructor might throw if SDK not happy, or if params invalid?
        # Usually it just initializes.
        engine = rithmic_ext.PyREngine(params)
        print("REngine created.")

        # Callbacks
        cb = MyCallbacks()
        
        # Login Params
        lparams = rithmic_ext.PyLoginParams()
        lparams.set_md_user("TEST")
        lparams.set_md_password("TEST")
        lparams.set_md_cnnct_pt("TEST_POINT")
        
        # Testing logic method (will fail to connect but should run)
        print("Attempting login (expected failure)...")
        success, code = engine.login(lparams, cb)
        print(f"Login result: {success}, Code: {code}")

        # Cleanup handled by GC
        print("Test Complete.")

    except Exception as e:
        print(f"FAILED: {e}")
        sys.exit(1)

if __name__ == "__main__":
    test_instantiation()
