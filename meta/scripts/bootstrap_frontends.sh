#!/usr/bin/env bash
set -euo pipefail

# --- Guard: run from repo root (must see ./client and ./server)
test -d client && test -d server || { echo "Run from repo root (must see ./client and ./server)"; exit 1; }

echo "==> Installing system prereqs for Tauri on Ubuntu..."
if command -v apt >/dev/null 2>&1; then
  # Base toolchain + GTK/webview bits commonly needed by Tauri on Linux
  sudo apt update
  sudo apt install -y \
    build-essential pkg-config libssl-dev \
    libgtk-3-dev libayatana-appindicator3-dev librsvg2-dev \
    libwebkit2gtk-4.1-dev curl
else
  echo "apt not found — skip system deps step (you can install equivalent packages for your distro)."
fi

echo "==> Ensuring Rust toolchain via rustup..."
if ! command -v rustup >/dev/null 2>&1; then
  curl https://sh.rustup.rs -sSf | sh -s -- -y
  export PATH="$HOME/.cargo/bin:$PATH"
fi
rustup update stable
rustup default stable

# ---------------------------------------------
# Web (React + Vite + Tailwind + shadcn/ui)
# ---------------------------------------------
WEB="client/web"
if [ -d "$WEB" ]; then
  echo "==> Installing web deps..."
  cd "$WEB"
  # Ensure plugin + Tailwind + shadcn deps exist
  # (idempotent: re-adding is fine)
  npm pkg set type="module" >/dev/null
  npm pkg set devDependencies."@vitejs/plugin-react"="^4.3.4" >/dev/null
  npm pkg set devDependencies.tailwindcss="^3.4.0" postcss="^8.4.47" autoprefixer="^10.4.20" >/dev/null
  npm pkg set dependencies."class-variance-authority"="^0.7.0" >/dev/null
  npm pkg set dependencies."clsx"="^2.1.0" >/dev/null
  npm pkg set dependencies."tailwind-merge"="^2.5.2" >/dev/null
  npm pkg set dependencies."@radix-ui/react-slot"="^1.0.2" >/dev/null

  # Make sure Tailwind config exists (safe overwrite if missing)
  test -f tailwind.config.ts || cat > tailwind.config.ts <<'TWCFG'
import type { Config } from "tailwindcss";
export default {
  content: ["./index.html", "./src/**/*.{ts,tsx}"],
  theme: { extend: {} },
  plugins: [],
} satisfies Config;
TWCFG

  test -f postcss.config.js || cat > postcss.config.js <<'POSTCSS'
export default {
  plugins: {
    tailwindcss: {},
    autoprefixer: {},
  },
}
POSTCSS

  mkdir -p src/styles
  test -f src/styles/tailwind.css || cat > src/styles/tailwind.css <<'CSS'
@tailwind base;
@tailwind components;
@tailwind utilities;
CSS

  # Ensure entry imports Tailwind once (idempotent)
  if ! grep -q 'tailwind.css' src/main.tsx 2>/dev/null; then
    sed -i '1i import "./styles/tailwind.css";' src/main.tsx || true
  fi

  npm install
  cd - >/dev/null
else
  echo "WARN: $WEB not found; skipping web setup."
fi

# ---------------------------------------------
# Desktop (Tauri + React + Vite) — local CLI
# ---------------------------------------------
DESK="client/desktop/tauri-app"
if [ -d "$DESK" ]; then
  echo "==> Installing desktop (Tauri) deps..."
  cd "$DESK"
  # Ensure local CLI + API installed as dev deps (no global needed)
  npm pkg set devDependencies."@tauri-apps/cli"="^2.0.0" >/dev/null
  npm pkg set dependencies."@tauri-apps/api"="^2.0.0" >/dev/null

  # Ensure scripts exist (idempotent)
  npm pkg set scripts.dev="vite" >/dev/null
  npm pkg set scripts.build="vite build" >/dev/null
  npm pkg set scripts.tauri:dev="tauri dev" >/dev/null
  npm pkg set scripts.tauri:build="tauri build" >/dev/null

  npm install
  cd - >/dev/null
else
  echo "WARN: $DESK not found; skipping desktop setup."
fi

# ---------------------------------------------
# Mobile (Expo) + TV (Expo)
# ---------------------------------------------
for APP in "client/mobile" "client/tv"; do
  if [ -d "$APP" ]; then
    echo "==> Installing deps for $APP ..."
    cd "$APP"
    # Keep light; expo will add more on first run if needed
    npm install
    cd - >/dev/null
  else
    echo "WARN: $APP not found; skipping."
  fi
done

# ---------------------------------------------
# Create multi-instance runner for desktop
# ---------------------------------------------
mkdir -p meta/scripts
cat > meta/scripts/tauri_multi_instance.sh <<'RUNNER'
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
RUNNER
chmod +x meta/scripts/tauri_multi_instance.sh

echo
echo "==> All set."
echo
echo "Web:"
echo "  cd client/web && npm run dev    # Tailwind + shadcn + Vite"
echo
echo "Desktop (single instance):"
echo "  cd client/desktop/tauri-app && npm run tauri:dev"
echo
echo "Desktop (multi instance):"
echo "  meta/scripts/tauri_multi_instance.sh 5173 inst1"
echo "  meta/scripts/tauri_multi_instance.sh 5174 inst2"
echo
echo "Mobile:"
echo "  cd client/mobile && npm run start"
echo
echo "TV:"
echo "  cd client/tv && npm run android   # with Android TV emulator/device"
