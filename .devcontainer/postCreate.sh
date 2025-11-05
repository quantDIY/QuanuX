#!/usr/bin/env bash
set -euo pipefail

echo "🔧 Installing Tauri + WebKit build deps..."
sudo apt-get update
sudo apt-get install -y \
  build-essential curl wget pkg-config libssl-dev \
  libwebkit2gtk-4.1-0 libwebkit2gtk-4.1-dev \
  libgtk-3-0 libgtk-3-dev \
  libayatana-appindicator3-1 libayatana-appindicator3-dev \
  librsvg2-2 librsvg2-dev \
  xz-utils ca-certificates

echo "⚙️ Setting up pnpm..."
corepack enable
corepack prepare pnpm@10.20.0 --activate

echo "📦 Installing workspace dependencies..."
pnpm install --frozen-lockfile || pnpm install

echo "✅ Codespace ready! Common commands:
  • pnpm -C client/web dev
  • pnpm -C client/desktop/tauri-app dev
  • pnpm -C client/desktop/tauri-app tauri:dev"
