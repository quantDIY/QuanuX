#!/bin/bash
set -e

# QuanuX Execution Node Installer
# Usage: curl -sL ... | sudo bash -s -- --token <token> --hub <url>

HUB_URL=""
TOKEN=""
INSTALL_DIR="/usr/local/bin"
CONFIG_DIR="/etc/quanux-node"
SERVICE_FILE="/etc/systemd/system/quanux-node.service"

# Pars args
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --hub) HUB_URL="$2"; shift ;;
        --token) TOKEN="$2"; shift ;;
        *) echo "Unknown parameter: $1"; exit 1 ;;
    esac
    shift
done

if [ -z "$HUB_URL" ] || [ -z "$TOKEN" ]; then
    echo "Error: --hub and --token are required."
    exit 1
fi

echo "🚀 Installing QuanuX Execution Node..."

# 1. Download Binary (Mocked for now, assuming local build or release URL)
# In production: curl -L https://github.com/QuanuX/QuanuX/releases/download/latest/quanux-node-linux-amd64 -o $INSTALL_DIR/quanux-node
echo "-> Fetching binary..."
# For prototype, we check if it exists or warn
if [ ! -f "$INSTALL_DIR/quanux-node" ]; then
    echo "Warning: Binary not found in $INSTALL_DIR. Please copy the built binary there manually for this prototype."
else
    chmod +x "$INSTALL_DIR/quanux-node"
fi

# 2. Install NATS Server
if ! command -v nats-server &> /dev/null; then
    echo "-> Installing nats-server..."
    # Simplified installation for Debian/Ubuntu
    if command -v apt-get &> /dev/null; then
        curl -L https://github.com/nats-io/nats-server/releases/download/v2.10.7/nats-server-v2.10.7-linux-amd64.tar.gz -o nats.tar.gz
        tar -xzf nats.tar.gz
        mv nats-server-v2.10.7-linux-amd64/nats-server $INSTALL_DIR/
        rm -rf nats.tar.gz nats-server-*
    else
        echo "Warning: Could not install nats-server automatically. Please install manually."
    fi
fi

# 3. Create User
if ! id -u quanux &>/dev/null; then
    useradd -r -s /bin/false quanux
fi

# 4. Register
echo "-> Registering node..."
mkdir -p /home/quanux/.quanux-node
chown quanux:quanux /home/quanux/.quanux-node
# We run register as the quanux user
sudo -u quanux $INSTALL_DIR/quanux-node register --hub "$HUB_URL" --token "$TOKEN" --name "$(hostname)"

# 5. Install Systemd Service
echo "-> Configuring Systemd..."
cat > "$SERVICE_FILE" <<EOF
[Unit]
Description=QuanuX Execution Node (High-Performance Engine)
After=network.target

[Service]
User=quanux
Group=quanux
ExecStart=$INSTALL_DIR/quanux-node start
Restart=always
RestartSec=5
LimitNOFILE=65536

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable quanux-node
systemctl start quanux-node

echo "✅ Installation Complete! Node is running."
