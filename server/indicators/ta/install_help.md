TA-Lib Install Help (Ubuntu 22.04 / Linux)
==========================================

Overview
--------
QuanuX indicators prefer TA-Lib for speed. If TA-Lib is not available, the loader can fall back to pandas_ta if you install it. This file shows both options and how to verify which backend is active.

Verify Which Backend QuanuX Will Use
------------------------------------
Run these with your venv active and repo root on PYTHONPATH.

$ export PYTHONPATH="$PWD:$PYTHONPATH"
$ quanuxctl indicators probe

Expected example when TA-Lib is active:
{ "backend": "ta-lib", "version": "0.6.x", "ready": true, "note": "" }

Preferred: Install the TA-Lib Python Wheel
------------------------------------------
This is the fastest path on many Linux environments (Ubuntu 22.04 + Python 3.11).

$ python -m pip install -U pip
$ python -m pip install TA-Lib

Verify the import:
$ python - <<'PY'
import talib, sys
print("TA-Lib", talib.__version__, "ok on", sys.version.split()[0])
PY

If the above fails with no matching distribution or a binary issue, use the build from source section below.

Build From Source (C library then Python wrapper)
-------------------------------------------------
Install system build tools:

$ sudo apt update
$ sudo apt install -y build-essential wget tar

Download and build the C library:

$ cd /tmp
$ wget -O ta-lib-0.4.0-src.tar.gz https://sourceforge.net/projects/ta-lib/files/ta-lib/0.4.0/ta-lib-0.4.0-src.tar.gz/download
$ tar -xzf ta-lib-0.4.0-src.tar.gz
$ cd ta-lib
$ ./configure --prefix=/usr/local
$ make -j"$(nproc)"
$ sudo make install
$ sudo ldconfig

Install the Python wrapper using source build:

$ python -m pip install --no-binary :all: TA-Lib

Verify again:

$ python - <<'PY'
import talib, sys
print("TA-Lib", talib.__version__, "ok on", sys.version.split()[0])
PY

Fallback Backend: pandas_ta
---------------------------
Use this only if you do not want native TA-Lib.

Important: pip package name is pandas-ta but import name is pandas_ta.

$ python -m pip install pandas-ta
$ python - <<'PY'
import pandas_ta as pta
print("pandas_ta", pta.__version__, "ready")
PY

Note: If pip cannot find pandas-ta for your Python version, consider using TA-Lib instead or create a conda environment and install from conda-forge.

Conda Users (Optional)
----------------------
TA-Lib via conda-forge:

$ conda install -c conda-forge ta-lib

Fallback via conda-forge:

$ conda install -c conda-forge pandas-ta

Quick Functional Test via CLI
-----------------------------
Assumes NATS and the nats_agent are running, venv is active, and repo root is your CWD.

$ export PYTHONPATH="$PWD:$PYTHONPATH"
$ python -m pip install -e server/cli
$ quanuxctl indicators probe
$ quanuxctl indicators demo-sma --length 30 --rows 240

You should see a small table with the last rows of "close" and "SMA_30".

Common Issues and Fixes
-----------------------
Issue: "No matching distribution found for pandas-ta"
Fix: pandas-ta may not publish wheels for every Python version. Prefer TA-Lib or use conda-forge.

Issue: "error while loading shared libraries: libta_lib.so"
Fix: run sudo ldconfig. Ensure /usr/local/lib is visible to the dynamic linker.

Issue: Wrong interpreter or venv mismatch
Fix: confirm you are using the expected Python:
$ which python
$ python -c "import sys; print(sys.executable)"
$ python -m pip show TA-Lib

Where This Fits In QuanuX
-------------------------
Loader: server/indicators/ta/loader.py
Indicators API: server/indicators/ta/indicators.py
CLI commands: server/cli/src/quanuxctl/commands/indicators.py

The loader attempts TA-Lib first and falls back to pandas_ta if present. If neither is installed, it returns a helpful message with install guidance.

