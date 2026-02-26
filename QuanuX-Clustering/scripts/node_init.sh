#!/usr/bin/env bash
# QuanuX Node Initialization: Operation "Foundation One"
# The Outer Russian Doll

set -e

echo "[QuanuX-Clustering] Initiating Outer Russian Doll (Operation: Foundation One)..."

# 2. The OS Scrub
echo "[QuanuX-Clustering] OS Scrub: Updating and hardening Ubuntu 22.04 LTS..."
export DEBIAN_FRONTEND=noninteractive
apt-get update
# Note: upgrade is commented out in typical dev scripts for speed, but required by specs.
apt-get upgrade -y
apt-get install -y build-essential curl git ufw

# 2. The Nutrient Layer
echo "[QuanuX-Clustering] Nutrient Layer: Installing set-quanux-node..."
if command -v conda &> /dev/null; then
    conda install -y -c quanux-repo set-quanux-node
else
    echo "[!] Warning: conda not found. Please install Anaconda/Miniconda first to enable the Nutrient Layer."
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

# 5. The Success Signal
echo "QUANUX_NODE_READY"
exit 0
