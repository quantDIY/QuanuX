#!/bin/bash

# Clean up any existing NATS or Node processes on exit
trap "kill 0" EXIT

echo "----------------------------------------"
echo "   QuanuX HFT Stack Initialization"
echo "----------------------------------------"

# 1. Start NATS Server
echo "[1/3] Starting NATS Server..."
if command -v nats-server &> /dev/null; then
    nats-server -p 4222 &
    NATS_PID=$!
    echo "      NATS Server running (PID: $NATS_PID)"
else
    echo "      Error: nats-server not found in PATH."
    exit 1
fi

sleep 2

# 2. Start HFT Node (Simulated Exchange + Strategy)
echo "[2/3] Starting Hyper-Node (PingPong Strategy)..."
# Ensure we are in project root
cd "$(dirname "$0")/.."

NODE_BIN="./execution-node/cpp/build/quanux_node"
STRATEGY="./execution-node/cpp/build/ping_pong.dylib"

if [ ! -f "$NODE_BIN" ]; then
    echo "      Error: Node binary not found. Build first!"
    exit 1
fi

# Run Node (It will now connect to NATS)
$NODE_BIN $STRATEGY
