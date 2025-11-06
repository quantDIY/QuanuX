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

echo "🧰 Corepack/pnpm"
corepack enable || true
corepack prepare pnpm@10.20.0 --activate || true

echo "📦 workspace install (best-effort)"
pnpm install --frozen-lockfile || pnpm install

echo "🧪 headless helper"
sudo tee /usr/local/bin/tauri-dev-headless >/dev/null <<'EOS'
#!/usr/bin/env bash
set -euo pipefail
export NO_AT_BRIDGE=1
export GTK_USE_PORTAL=0
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/tmp/runtime-$UID}"
mkdir -p "$XDG_RUNTIME_DIR"

exec dbus-run-session -- \
  xvfb-run -s "-screen 0 1280x800x24" \
  pnpm -C client/desktop/tauri-app run tauri:dev
EOS
sudo chmod +x /usr/local/bin/tauri-dev-headless

echo "📝 add tauri:dev:headless script to desktop app"
node - <<'NODE'
const fs=require("fs"), p="client/desktop/tauri-app/package.json";
if (fs.existsSync(p)) {
  const j=JSON.parse(fs.readFileSync(p,"utf8"));
  j.scripts ||= {};
  j.scripts["tauri:dev:headless"] = "NO_AT_BRIDGE=1 GTK_USE_PORTAL=0 dbus-run-session -- xvfb-run -s \"-screen 0 1280x800x24\" tauri dev";
  fs.writeFileSync(p, JSON.stringify(j,null,2)+"\n");
  console.log("package.json updated with tauri:dev:headless");
} else {
  console.log("skip: desktop package.json not found");
}
NODE

echo "✅ postCreate complete"
