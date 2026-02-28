#!/bin/bash
set -e
echo "Installing NATS Server..."
apt-get install -y unzip
curl -L https://github.com/nats-io/nats-server/releases/download/v2.10.11/nats-server-v2.10.11-linux-amd64.zip -o /tmp/nats.zip
cd /tmp && unzip nats.zip
cp nats-server-v2.10.11-linux-amd64/nats-server /usr/local/bin/

cat << 'EOF' > /etc/systemd/system/nats-server.service
[Unit]
Description=NATS Server
After=network.target

[Service]
ExecStart=/usr/local/bin/nats-server -c /etc/quanux/nats/nats-server.conf
Restart=always

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable --now nats-server
systemctl restart quanux-envoy
echo "NATS and Envoy Restarted."
