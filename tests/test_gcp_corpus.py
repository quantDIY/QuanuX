import pytest
import glob
import sys
import os

# Add QuanuX-Annex and the project root to path
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../QuanuX-Annex')))
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from gcp_transpiler import QuanuXDuckToBQTranspiler, TranspilationError

FIXTURE_DIR = os.path.join(os.path.dirname(__file__), "fixtures", "gcp_sql")
ALLOWED_FIXTURES = glob.glob(os.path.join(FIXTURE_DIR, "allowed", "*.sql"))
REJECTED_FIXTURES = glob.glob(os.path.join(FIXTURE_DIR, "rejected", "*.sql"))

@pytest.fixture(scope="module")
def transpiler():
    return QuanuXDuckToBQTranspiler()

@pytest.mark.parametrize("filepath", ALLOWED_FIXTURES, ids=[os.path.basename(f) for f in ALLOWED_FIXTURES])
def test_allowed_corpus(transpiler, filepath):
    with open(filepath, "r") as f:
        query = f.read().strip()
    
    # Allowed queries must transpile without error
    try:
        transpiler.transpile(query)
    except TranspilationError as e:
        pytest.fail(f"Allowed corpus query {os.path.basename(filepath)} failed transpilation erroneously: {e}")

@pytest.mark.parametrize("filepath", REJECTED_FIXTURES, ids=[os.path.basename(f) for f in REJECTED_FIXTURES])
def test_rejected_corpus(transpiler, filepath):
    with open(filepath, "r") as f:
        query = f.read().strip()
        
    # Rejected queries must unequivocally raise a TranspilationError
    with pytest.raises(TranspilationError) as exc_info:
        transpiler.transpile(query)
        
    # Ensure they hit the proper bounded logic
    assert "Prototype Matrix Boundary Violation" not in str(exc_info.value) # CLI tag, inner trace should be cleaner
    assert "Unsupported construct" in str(exc_info.value)
