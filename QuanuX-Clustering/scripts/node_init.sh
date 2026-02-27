#!/usr/bin/env bash
# QuanuX Node Initialization: Operation "Foundation One"
# The Outer Russian Doll

set -e

echo "[QuanuX-Clustering] Initiating Outer Russian Doll (Operation: Foundation One)..."

TIER=1
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --tier) TIER="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done
echo "[QuanuX-Clustering] Initializing Node as Tier $TIER"

# 2. The OS Scrub
echo "[QuanuX-Clustering] OS Scrub: Updating and hardening Ubuntu 22.04 LTS..."
export DEBIAN_FRONTEND=noninteractive
apt-get update
# Note: upgrade is commented out in typical dev scripts for speed, but required by specs.
apt-get upgrade -y
apt-get install -y build-essential curl git ufw

# 2. The Nutrient Layer
if [ "$TIER" -eq 1 ]; then
    echo "[QuanuX-Clustering] Nutrient Layer: Installing set-quanux-node..."
    if command -v conda &> /dev/null; then
        conda install -y -c quanux-repo set-quanux-node
    else
        echo "[!] Warning: conda not found. Please install Anaconda/Miniconda first to enable the Nutrient Layer."
    fi
else
    echo "[QuanuX-Clustering] Edge Layer (Tier $TIER): Skipping Conda. Installing C++20 toolchains and Native Envoy dependencies..."
    apt-get install -y cmake libnats-dev libspdlog-dev
fi

# 2. The RSA Handshake
echo "[QuanuX-Clustering] RSA Handshake: Generating 4096-bit RSA keypair..."
KEY_DIR="/etc/quanux/keys"
mkdir -p "$KEY_DIR"
if [ ! -f "$KEY_DIR/node_rsa" ]; then
    ssh-keygen -t rsa -b 4096 -N "" -f "$KEY_DIR/node_rsa"
    echo "[+] RSA keypair generated at $KEY_DIR/node_rsa"
else
    echo "[+] RSA keypair already exists at $KEY_DIR/node_rsa"
fi

# 3. The NATS Grid Integration
echo "[QuanuX-Clustering] NKey Generation: Creating Ed25519 NKey pair..."
NKEY_FILE="$KEY_DIR/node.nk"
if [ ! -f "$NKEY_FILE" ]; then
    # Generate an NKEY if nk is available, or use a placeholder string for the grid
    if command -v nk &> /dev/null; then
        nk -gen server -pubout > "$NKEY_FILE"
        echo "[+] NKey generated at $NKEY_FILE"
    else
        echo "[!] nk utility not found. Writing dummy NKEY for now (Please install nkeys tool)."
        echo "SUA" > "$NKEY_FILE"
    fi
else
    echo "[+] NKey already exists at $NKEY_FILE"
fi

echo "[QuanuX-Clustering] Cluster Binding: Configuring local nats-server.conf..."
NATS_CONF_DIR="/etc/quanux/nats"
mkdir -p "$NATS_CONF_DIR"
NATS_CONF="$NATS_CONF_DIR/nats-server.conf"

cat << 'EOF' > "$NATS_CONF"
# QuanuX Node NATS Configuration
port: 4222
server_name: "quanux-node-bootstrap"

cluster {
  name: "quanux-grid"
  listen: "0.0.0.0:6222"
  routes: [
    "nats-route://seed.quanux.local:6222"
  ]
}
EOF
echo "[+] nats-server.conf configured at $NATS_CONF"

echo "[QuanuX-Clustering] Security: Configuring UFW for NATS traffic..."
ufw allow 4222/tcp comment 'NATS Client'
ufw allow 6222/tcp comment 'NATS Cluster Route Handshaking'
ufw --force enable
echo "[+] UFW configured for NATS Grid"

# 4. Systemd Service Synthesis
if [ "$TIER" -eq 1 ]; then
    echo "[QuanuX-Clustering] Fast-Nerve Synthesis: Generating quanux-nerve.service..."
    # Robust conda path detection
    if command -v conda &> /dev/null; then
        CONDA_BASE=$(conda info --base)
        PYTHON_EXE="$CONDA_BASE/envs/quanux-node/bin/python"
    else
        # Fallback assuming default locations
        PYTHON_EXE="$HOME/anaconda3/envs/quanux-node/bin/python"
        echo "[!] conda not in PATH, falling back to $PYTHON_EXE."
    fi

    SERVICE_FILE="/tmp/quanux-nerve.service"
    cat << EOF > "$SERVICE_FILE"
[Unit]
Description=QuanuX Fast-Nerve FastAPI Gateway
After=network.target nats-server.service

[Service]
Type=simple
User=$USER
WorkingDirectory=$(pwd)
Environment="CONDA_PREFIX=${PYTHON_EXE%/*}/.."
Environment="PATH=${PYTHON_EXE%/*}:/usr/bin:/bin"
ExecStart=$PYTHON_EXE -m uvicorn server.api.main:app --host 0.0.0.0 --port 8080
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

    echo "[QuanuX-Clustering] Installing and Starting Fast-Nerve..."
    sudo mv "$SERVICE_FILE" /etc/systemd/system/quanux-nerve.service
    sudo systemctl daemon-reload
    sudo systemctl enable --now quanux-nerve
    echo "[+] quanux-nerve.service is Sovereign."
else
    echo "[QuanuX-Clustering] Native Envoy Synthesis: Compiling C++20 Shell..."
    ENVOY_SRC_DIR="$(pwd)/QuanuX-Clustering/src/shells/native-envoy"
    mkdir -p "$ENVOY_SRC_DIR/build"
    cd "$ENVOY_SRC_DIR/build"
    cmake ..
    cmake --build .
    sudo cp quanux-envoy /usr/local/bin/
    cd - > /dev/null

    SERVICE_FILE="/tmp/quanux-envoy.service"
    # CPUAffinity=0 strictly assigns it to core 0 as the management core
    cat << EOF > "$SERVICE_FILE"
[Unit]
Description=QuanuX Native Envoy Daemon
After=network.target nats-server.service

[Service]
Type=simple
User=$USER
WorkingDirectory=/usr/local/bin
ExecStart=/usr/local/bin/quanux-envoy
CPUAffinity=0
Restart=always
RestartSec=1

[Install]
WantedBy=multi-user.target
EOF

    echo "[ENV] Configuring passwordless sudo for Native Envoy telemetry..."
    SUDOERS_FILE="/tmp/quanux-envoy-sudoers"
    cat << EOF > "$SUDOERS_FILE"
$USER ALL=(ALL) NOPASSWD: /bin/systemctl restart quanux-envoy, /bin/systemctl stop quanux-envoy, /bin/systemctl start quanux-envoy, /bin/systemctl restart nats-server
EOF
    sudo chown root:root "$SUDOERS_FILE"
    sudo chmod 0440 "$SUDOERS_FILE"
    sudo mv "$SUDOERS_FILE" /etc/sudoers.d/quanux-envoy

    echo "[QuanuX-Clustering] Installing and Starting Native Envoy..."
    sudo mv "$SERVICE_FILE" /etc/systemd/system/quanux-envoy.service
    sudo systemctl daemon-reload
    sudo systemctl enable --now quanux-envoy
    echo "[+] quanux-envoy.service is Sovereign (Tier $TIER)."
fi
# 5. The Success Signal
echo "QUANUX_NODE_READY"

# 6. Post-Flight Hook: Neural Tap & Auto-Suture
# We read from /dev/tty in case the script is piped via bash -s
echo "[?] <$(hostname)> init successful. Run health probe now? [Y/n]"
if read -r -t 15 PROBE_RESP < /dev/tty; then
    :
else
    echo "[!] Interaction timeout. Assuming 'Y' for health probe..."
    PROBE_RESP="y"
fi

if [[ "$PROBE_RESP" =~ ^[Yy]$ || -z "$PROBE_RESP" ]]; then
    echo "[QuanuX-Clustering] Executing Health Probe (Neural Tap)..."
    set +e
    quanuxctl probe
    PROBE_STATUS=$?
    set -e
    
    if [ $PROBE_STATUS -ne 0 ]; then
        echo "[!] ⚠️ Probe failed with exit code $PROBE_STATUS. Attempt Auto-Suture? [Y/n]"
        if read -r -t 15 AUTO_SUTURE_RESP < /dev/tty; then
            :
        else
            echo "[!] Interaction timeout. Assuming 'Y' for Auto-Suture..."
            AUTO_SUTURE_RESP="y"
        fi

        if [[ "$AUTO_SUTURE_RESP" =~ ^[Yy]$ || -z "$AUTO_SUTURE_RESP" ]]; then
            echo "[QuanuX-Clustering] Executing Auto-Suture (Fix)..."
            quanuxctl probe --fix
        else
            echo "[!] Auto-Suture aborted. Manual intervention required."
        fi
    else
        echo "[+] Health probe passed. Node is Nominal."
    fi
else
    echo "[i] Health probe skipped."
fi

exit 0
