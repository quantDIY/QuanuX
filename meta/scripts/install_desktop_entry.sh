#!/usr/bin/env bash
set -euo pipefail
SRC="meta/manifests/desktop/quanux-desktop.desktop"
TARGET="${XDG_DATA_HOME:-$HOME/.local/share}/applications/quanux-desktop.desktop"
mkdir -p "$(dirname "$TARGET")"
cp "$SRC" "$TARGET"
update-desktop-database "$(dirname "$TARGET")" >/dev/null 2>&1 || true
echo "Installed launcher: $TARGET"

AUTOSTART="${XDG_CONFIG_HOME:-$HOME/.config}/autostart"
mkdir -p "$AUTOSTART"
cp "$SRC" "$AUTOSTART/quanux-desktop.desktop"
sed -i 's/npm run tauri:dev/npm run tauri:dev \& disown/g' "$AUTOSTART/quanux-desktop.desktop" || true
echo "Autostart entry: $AUTOSTART/quanux-desktop.desktop"
