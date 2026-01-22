# QuanuX Developer Setup

## Python Environment
QuanuX relies on a Python virtual environment for the server, CLI, and agent tools.

### 1. Prerequisites
- Python 3.10+
- Anaconda or Miniconda (recommended) or standard venv

### 2. Setup
```bash
# Create and activate venv
python3 -m venv venv
source venv/bin/activate

# Install Dependencies
pip install -r requirements.txt
```

### 3. installing the CLI (quanuxctl)
To use the `quanuxctl` tool in development mode with the local codebase:

```bash
# Install CLI in editable mode
pip install -e server/cli

# Set PYTHONPATH to include the project root (crucial for imports like 'server.security...')
export PYTHONPATH=$PWD
```

### 4. Verification
```bash
quanuxctl ext list
```

## C++ Extensions
To build C++ extensions (like Rithmic), you need `cmake`.

### macOS
```bash
brew install cmake
```

### Linux
```bash
sudo apt-get install cmake
```
