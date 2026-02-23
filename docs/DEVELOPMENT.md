# QuanuX Developer Setup

## Quick Start

### 1. Prerequisites
- **OS**: macOS (Apple Silicon recommended) or Linux (x86_64).
- **C++**: Compiler supporting C++20 (Clang 15+ or GCC 11+).
- **Build System**: CMake 3.20+.
- **Python**: 3.10+ (Anaconda or venv).
- **Messaging**: `nats-server` (recommended) or NATS access.

### 2. Environment Setup
```bash
# Create Python environment
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Install CLI (quanuxctl) in editable mode
# IMPORTANT: Use PYTHONPATH=. to include the project root for imports
export PYTHONPATH=$PWD
pip install -e server/cli
```

### 3. Verify CLI
```bash
# Check if quanuxctl is working
quanuxctl --version
```

---

## Architecture Components

### 1. Execution Node (C++)
The core HFT engine is located in `execution-node/cpp`. 

**Building the Node:**
```bash
cd execution-node/cpp
mkdir -p build && cd build
cmake ..
make -j$(nproc)
```
*Note: This will automatically fetch dependencies (NATS C client, etc.) via FetchContent.*

**Running:**
```bash
# From execution-node/cpp/build
./quanux_node
```

### 4. Extensions (Data & Storage)
Extensions for Databento and DuckDB are located in `extensions/cpp/`. They are typically built as part of the main `quanux_node` build or separately for testing.

**Databento Setup:**
Ensure your API key is available in your shell or `secrets` store:
```bash
export DATABENTO_API_KEY="YOUR_DATABENTO_KEY"
```

**Rithmic Setup:**
1. **SSL Certs**: Run `python3 scripts/setup_rithmic_env.py` to link the required SSL certificates to your runtime environment.
2. **Credentials**: Use `python3 scripts/manage_keyring.py set <user>` to store your credentials securely.
3. **Usage**: The server router will automatically pick up credentials from the keyring.


### 5. Quanux Indicators (C++ Core)
The core indicator library (TA-Lib replacement) is in `server/indicators`.
It creates a C++ shared library and a Python extension.

```bash
cd server/indicators
mkdir build && cd build
cmake ..
make
```

### 3. NATS Integration
NATS (via `nats.c`) handles all telemetry. Ensure a NATS server is running on `localhost:4222`:

```bash
# Install (macOS)
brew install nats-server

# Run
nats-server
```

---

## Common Workflows

### Running the Backtester (Mock Data)
See `technical_whitepaper.md` for architectural details on the backtesting loop. To simulate a feed:

```bash
# Start NATS
nats-server -p 4222 &

# Run Node with Mock Strategy
./execution-node/cpp/build/quanux_node <path_to_strategy.so>
```

### Adding New Dependencies
- **Python**: Add to `requirements.txt`.
- **C++**: Add to `execution-node/cpp/CMakeLists.txt` using `FetchContent` or `find_package`.

---

## Troubleshooting

### "Module not found: server" in CLI
Ensure you are running `quanuxctl` with `PYTHONPATH=.` from the project root, or that the package is installed in editable mode properly.

### CMake "Missing header"
If headers like `nats.h` are missing, ensure the `FetchContent_MakeAvailable(cnats)` step succeeded in the CMake output. Try clearing the `build/` directory and re-running `cmake ..`.

---

## Python Extension Guidelines

### Robust Packaging (CI/CD)
When writing `setup.py` files for extensions (Cython/PyBind11), you **must** handle missing build dependencies gracefully. CI environments often run dependency checks without installing build tools like Cython or Numpy.

**Requirements:**
1. Wrap `Cython` and `numpy` imports in `try-except ImportError` blocks.
2. Only define `Extension` objects if dependencies are present.
3. Ensure `python setup.py --name` succeeds in a bare environment.

**Example Pattern:**
```python
try:
    from Cython.Build import cythonize
    import numpy
except ImportError:
    cythonize = None

ext_modules = []
if cythonize:
    # Define extensions
    ext_modules = cythonize([...])

setup(
    name="my_extension",
    ext_modules=ext_modules
)
```
For more details, see the agent skill `python_packaging`.
