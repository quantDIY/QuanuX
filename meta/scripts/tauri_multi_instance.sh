#!/usr/bin/env bash
set -euo pipefail

# Run a new Tauri dev instance on a unique port and with distinct app dir.
# Usage: meta/scripts/tauri_multi_instance.sh [PORT] [APPDIR_SUFFIX]
PORT="${1:-5173}"
SUFFIX="${2:-inst1}"

APP="client/desktop/tauri-app"
test -d "$APP" || { echo "No $APP found"; exit 1; }

# Distinct dev server port for Vite
export VITE_PORT="$PORT"
# Optional: different profile dir (so instances don't stomp each other)
export TAURI_DEV_WATCHER_IGNORE="dist"
export TAURI_PRIVATE_DIR=".tauri-$SUFFIX"

cd "$APP"
# Kick vite on chosen port and spawn tauri dev that points at it
# Run vite in background, then tauri dev in foreground
npx vite --port "$PORT" --host &  # background
VITE_PID=$!

# Ensure we kill Vite when we exit
trap 'kill $VITE_PID 2>/dev/null || true' EXIT

# Let vite warm up a touch
sleep 0.8

# Local tauri CLI via npx (uses devDependency)
npx tauri dev
