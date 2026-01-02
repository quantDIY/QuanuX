#!/bin/bash

# Install the CLI package
$PYTHON -m pip install ./server/cli --no-deps --ignore-installed -vv

# Copy the 'server' package to site-packages
# We need to make sure 'server' is treated as a package. 
# It has an __init__.py, so it is a package.
cp -r server $SP_DIR/

# Install missing dependencies via pip that might not be in conda-forge or are specific versions
# User must pip install google-generativeai mcp manually if needed
# $PYTHON -m pip install google-generativeai mcp --no-deps
