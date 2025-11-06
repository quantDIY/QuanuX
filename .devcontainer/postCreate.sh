#!/usr/bin/env bash
set -euo pipefail

export DEBIAN_FRONTEND=noninteractive

echo "🔧 apt-get update/upgrade..."
sudo apt-get -yq update
sudo apt-get -yq upgrade

echo "📚 Tauri deps (GTK/WebKit/portals + headless X + dbus)"
sudo apt-get -yq install \
  build-essential curl wget pkg-config libssl-dev \
  libwebkit2gtk-4.1-0 libwebkit2gtk-4.1-dev \
  libgtk-3-0 libgtk-3-dev \
  libayatana-appindicator3-1 libayatana-appindicator3-dev \
  librsvg2-2 librsvg2-dev \
  xvfb dbus-x11 x11-apps \
  xz-utils ca-certificates

echo "🧹 Fix cache ownership for vscode user (prevents Corepack EACCES)"
sudo mkdir -p /home/vscode/.cache/node/corepack/v1
sudo chown -R vscode:vscode /home/vscode/.cache

echo "🧰 Corepack / pnpm"
corepack enable || true
corepack prepare pnpm@10.20.0 --activate || true

echo "📦 workspace install (best-effort)"
pnpm install -w --frozen-lockfile || pnpm install -w || true

# --------------------------------------------------------------------
# 🧪 headless Tauri helper (quiet, portable, disables portals)
# --------------------------------------------------------------------
echo "🧪 Installing quiet headless Tauri launcher..."
sudo tee /usr/local/bin/tauri-dev-headless >/dev/null <<'EOS'
#!/usr/bin/env bash
set -euo pipefail

# Quieter, headless-friendly env
export NO_AT_BRIDGE=1
export GTK_USE_PORTAL=0
export XDG_DESKTOP_PORTAL_DIR=/dev/null
export WEBKIT_DISABLE_DMABUF_RENDERER=1
export GDK_BACKEND=x11
export GTK_A11Y=0
export XDG_CURRENT_DESKTOP=GNOME

# secure runtime dir to silence dbus complaints
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/tmp/xdg-runtime-$UID}"
rm -rf "$XDG_RUNTIME_DIR" || true
mkdir -p "$XDG_RUNTIME_DIR"
chmod 700 "$XDG_RUNTIME_DIR"

# Run Tauri under a private D-Bus and Xvfb
exec dbus-run-session -- \
  xvfb-run -a -s "-screen 0 1280x800x24 +extension RANDR" \
  pnpm -C client/desktop/tauri-app run tauri:dev
EOS
sudo chmod +x /usr/local/bin/tauri-dev-headless

# --------------------------------------------------------------------
# 🧩 Update desktop package.json with tauri:dev:headless script
# --------------------------------------------------------------------
echo "📝 add tauri:dev:headless script to desktop app"
node - <<'NODE'
const fs = require("fs");
const path = "client/desktop/tauri-app/package.json";
if (fs.existsSync(path)) {
  const pkg = JSON.parse(fs.readFileSync(path, "utf8"));
  pkg.scripts ||= {};
  pkg.scripts["tauri:dev:headless"] =
    "NO_AT_BRIDGE=1 GTK_USE_PORTAL=0 XDG_DESKTOP_PORTAL_DIR=/dev/null " +
    "WEBKIT_DISABLE_DMABUF_RENDERER=1 GDK_BACKEND=x11 GTK_A11Y=0 " +
    "dbus-run-session -- xvfb-run -a -s '-screen 0 1280x800x24 +extension RANDR' tauri dev";
  fs.writeFileSync(path, JSON.stringify(pkg, null, 2) + "\n");
  console.log("✅ package.json updated with tauri:dev:headless");
} else {
  console.log("⚠️  skip: desktop package.json not found");
}
NODE

echo "✅ postCreate complete (quiet headless Tauri configured)"
