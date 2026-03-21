#!/bin/bash
go build -o qxctl main.go
QX_VAULT_TOKEN=dummy.eyJjYXBhYmlsaXRpZXMiOlsiaW5zcGVjdCJdfQ.dummy ./qxctl vault status --output=json --target=gcp > sigint.out 2>&1 &
PID=$!
sleep 1
kill -s INT $PID
wait $PID 2>/dev/null

QX_VAULT_TOKEN=dummy.eyJjYXBhYmlsaXRpZXMiOlsiaW5zcGVjdCJdfQ.dummy ./qxctl vault status --output=json --target=gcp > sigterm.out 2>&1 &
PID=$!
sleep 1
kill -s TERM $PID
wait $PID 2>/dev/null

cat sigint.out
echo "==="
cat sigterm.out
