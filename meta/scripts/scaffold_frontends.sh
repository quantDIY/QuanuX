#!/usr/bin/env bash
set -euo pipefail

# repo root guard: must contain ./server
test -d server || { echo "Run from repo root (must see ./server)"; exit 1; }

# ---- shared paths ----
ROOT="$PWD"
CLIENT="$ROOT/client"
mkdir -p "$CLIENT"

# =====================================================================================
# client/web  — React + Vite (TypeScript)
# =====================================================================================
WEB="$CLIENT/web"
mkdir -p "$WEB/src/ui"
cat >"$WEB/package.json"<<'EOF'
{
  "name": "@quanux/web",
  "private": true,
  "version": "0.1.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "preview": "vite preview --host",
    "check": "tsc --noEmit"
  },
  "dependencies": {
    "react": "^18.3.1",
    "react-dom": "^18.3.1"
  },
  "devDependencies": {
    "@types/react": "^18.3.5",
    "@types/react-dom": "^18.3.0",
    "typescript": "^5.6.3",
    "vite": "^5.4.10"
  }
}
EOF

cat >"$WEB/index.html"<<'EOF'
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0"/>
    <title>QuanuX Web</title>
  </head>
  <body>
    <div id="root"></div>
    <script type="module" src="/src/main.tsx"></script>
  </body>
</html>
EOF

cat >"$WEB/src/main.tsx"<<'EOF'
import React from "react";
import { createRoot } from "react-dom/client";
import { App } from "./ui/App";

const root = createRoot(document.getElementById("root")!);
root.render(<App />);
EOF

cat >"$WEB/src/ui/App.tsx"<<'EOF'
import React from "react";
import { Health } from "./Health";

export const App: React.FC = () => {
  return (
    <div style={{ fontFamily: "Inter, system-ui, Arial", padding: 16 }}>
      <h1>QuanuX Web</h1>
      <p>Lightweight presentation layer. Most computation stays on backend.</p>
      <Health />
    </div>
  );
};
EOF

cat >"$WEB/src/ui/Health.tsx"<<'EOF'
import React from "react";

export const Health: React.FC = () => {
  const [status, setStatus] = React.useState<null | { ok: boolean; details?: any }>(null);
  const [err, setErr] = React.useState<string | null>(null);

  const url = (import.meta as any).env?.VITE_BRIDGE_HEALTH ?? "http://127.0.0.1:8077/health";

  const ping = React.useCallback(async () => {
    try {
      setErr(null);
      const res = await fetch(url, { headers: { "Accept": "application/json" } });
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const json = await res.json();
      setStatus({ ok: true, details: json });
    } catch (e: any) {
      setStatus({ ok: false });
      setErr(e?.message || "Request failed");
    }
  }, [url]);

  React.useEffect(() => { void ping(); }, [ping]);

  return (
    <div style={{ marginTop: 12, padding: 12, border: "1px solid #ddd", borderRadius: 8 }}>
      <strong>NATS bridge health</strong>
      <div style={{ marginTop: 8 }}>
        {status === null && <span>Checking...</span>}
        {status && status.ok && <pre style={{ whiteSpace: "pre-wrap" }}>{JSON.stringify(status.details, null, 2)}</pre>}
        {status && !status.ok && <span style={{ color: "crimson" }}>Unavailable {err ? `— ${err}` : ""}</span>}
      </div>
      <button onClick={ping} style={{ marginTop: 8, padding: "6px 10px", cursor: "pointer" }}>Recheck</button>
    </div>
  );
};
EOF

cat >"$WEB/vite.config.ts"<<'EOF'
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";

export default defineConfig({
  plugins: [react()],
  server: {
    port: 5173,
    host: true
  }
});
EOF

cat >"$WEB/tsconfig.json"<<'EOF'
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "ESNext",
    "jsx": "react-jsx",
    "moduleResolution": "Bundler",
    "strict": true,
    "skipLibCheck": true,
    "noEmit": true,
    "types": []
  },
  "include": ["src"]
}
EOF

cat >"$WEB/.env.example"<<'EOF'
# Bridge health endpoint (NATS-backed SignalR/Flask)
VITE_BRIDGE_HEALTH=http://127.0.0.1:8077/health
EOF

cat >"$WEB/.gitignore"<<'EOF'
node_modules
dist
.DS_Store
*.local
.env
EOF

cat >"$WEB/README.md"<<'EOF'
# QuanuX Web (React + Vite)
- `pnpm i` (or `npm i`, `yarn`)
- copy `.env.example` → `.env` and set `VITE_BRIDGE_HEALTH`
- `pnpm dev` then open the local dev URL
EOF

# =====================================================================================
# client/desktop/tauri-app — Tauri + React + Vite (minimal shell)
# =====================================================================================
TAURI="$CLIENT/desktop/tauri-app"
mkdir -p "$TAURI/src" "$TAURI/src-tauri/src"

cat >"$TAURI/package.json"<<'EOF'
{
  "name": "@quanux/desktop-tauri",
  "private": true,
  "version": "0.1.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "vite build",
    "tauri:dev": "tauri dev",
    "tauri:build": "tauri build"
  },
  "dependencies": {
    "react": "^18.3.1",
    "react-dom": "^18.3.1"
  },
  "devDependencies": {
    "@tauri-apps/cli": "^2.0.0",
    "@types/react": "^18.3.5",
    "@types/react-dom": "^18.3.0",
    "typescript": "^5.6.3",
    "vite": "^5.4.10"
  }
}
EOF

cat >"$TAURI/index.html"<<'EOF'
<!doctype html>
<html lang="en">
<head><meta charset="UTF-8" /><meta name="viewport" content="width=device-width, initial-scale=1.0"/><title>QuanuX Desktop</title></head>
<body>
  <div id="root"></div>
  <script type="module" src="/src/main.tsx"></script>
</body>
</html>
EOF

cat >"$TAURI/src/main.tsx"<<'EOF'
import React from "react";
import { createRoot } from "react-dom/client";

const App = () => (
  <div style={{ padding: 16 }}>
    <h1>QuanuX Desktop (Tauri)</h1>
    <p>Runs the same React/Vite app inside a native shell.</p>
  </div>
);

createRoot(document.getElementById("root")!).render(<App />);
EOF

cat >"$TAURI/vite.config.ts"<<'EOF'
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
export default defineConfig({ plugins: [react()] });
EOF

cat >"$TAURI/tsconfig.json"<<'EOF'
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "ESNext",
    "jsx": "react-jsx",
    "moduleResolution": "Bundler",
    "strict": true,
    "skipLibCheck": true,
    "noEmit": true
  },
  "include": ["src"]
}
EOF

cat >"$TAURI/src-tauri/tauri.conf.json"<<'EOF'
{
  "productName": "QuanuX Desktop",
  "version": "0.1.0",
  "build": {
    "frontendDist": "../dist",
    "devUrl": "http://localhost:5173"
  },
  "app": {
    "windows": [{ "title": "QuanuX", "width": 1200, "height": 800 }]
  }
}
EOF

cat >"$TAURI/src-tauri/Cargo.toml"<<'EOF'
[package]
name = "quanux_desktop"
version = "0.1.0"
edition = "2021"

[dependencies]
tauri = { version = "2", features = ["tray-icon"] }
serde = { version = "1", features = ["derive"] }
serde_json = "1"

[build-dependencies]
tauri-build = "2"
EOF

cat >"$TAURI/src-tauri/src/main.rs"<<'EOF'
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]
fn main() {
  tauri::Builder::default()
    .run(tauri::generate_context!())
    .expect("error while running QuanuX desktop");
}
EOF

cat >"$TAURI/.gitignore"<<'EOF'
node_modules
dist
src-tauri/target
*.log
.DS_Store
EOF

cat >"$TAURI/README.md"<<'EOF'
# QuanuX Desktop (Tauri)
Prereqs: Rust toolchain + `@tauri-apps/cli`.
- `pnpm i`
- in one terminal: `pnpm dev`
- in another: `pnpm tauri:dev`
EOF

# =====================================================================================
# client/mobile — Expo (React Native, TS)
# =====================================================================================
MOBILE="$CLIENT/mobile"
mkdir -p "$MOBILE"
cat >"$MOBILE/package.json"<<'EOF'
{
  "name": "@quanux/mobile",
  "private": true,
  "version": "0.1.0",
  "main": "index.js",
  "scripts": {
    "start": "expo start",
    "android": "expo run:android",
    "ios": "expo run:ios",
    "web": "expo start --web"
  },
  "dependencies": {
    "expo": "^51.0.0",
    "expo-status-bar": "~1.12.1",
    "react": "18.3.1",
    "react-native": "0.76.3"
  },
  "devDependencies": {
    "typescript": "^5.6.3"
  }
}
EOF

cat >"$MOBILE/app.json"<<'EOF'
{
  "expo": {
    "name": "QuanuX Mobile",
    "slug": "quanux-mobile",
    "scheme": "quanux",
    "android": { "package": "dev.quantdiy.quanux.mobile" },
    "ios": { "bundleIdentifier": "dev.quantdiy.quanux.mobile" }
  }
}
EOF

cat >"$MOBILE/App.tsx"<<'EOF'
import React from "react";
import { SafeAreaView, Text, View, Button } from "react-native";

export default function App() {
  const [pinged, setPinged] = React.useState<string>("(not yet)");
  const healthUrl = "http://10.0.2.2:8077/health"; // Android emulator -> host

  return (
    <SafeAreaView>
      <View style={{ padding: 16 }}>
        <Text style={{ fontSize: 22, fontWeight: "600" }}>QuanuX Mobile</Text>
        <Text>Backend/bridge status: {pinged}</Text>
        <Button title="Ping bridge" onPress={async () => {
          try {
            const r = await fetch(healthUrl);
            setPinged(r.ok ? "OK" : `HTTP ${r.status}`);
          } catch (e) {
            setPinged("error");
          }
        }} />
      </View>
    </SafeAreaView>
  );
}
EOF

cat >"$MOBILE/.gitignore"<<'EOF'
node_modules
.expo
dist
.DS_Store
EOF

cat >"$MOBILE/README.md"<<'EOF'
# QuanuX Mobile (Expo)
- Install Expo CLI (`npm i -g expo` or use npx)
- `pnpm i`
- `pnpm start` then open iOS/Android
EOF

# =====================================================================================
# client/tv — Expo w/ Android TV flags
# =====================================================================================
TV="$CLIENT/tv"
mkdir -p "$TV"
cat >"$TV/package.json"<<'EOF'
{
  "name": "@quanux/tv",
  "private": true,
  "version": "0.1.0",
  "main": "index.js",
  "scripts": {
    "start": "expo start",
    "android": "expo run:android"
  },
  "dependencies": {
    "expo": "^51.0.0",
    "react": "18.3.1",
    "react-native": "0.76.3"
  }
}
EOF

cat >"$TV/app.json"<<'EOF'
{
  "expo": {
    "name": "QuanuX TV",
    "slug": "quanux-tv",
    "android": {
      "package": "dev.quantdiy.quanux.tv",
      "tvBanner": "./assets/tv-banner.png",
      "adaptiveIcon": { "foregroundImage": "./assets/icon.png", "backgroundColor": "#111111" },
      "intentFilters": [{ "autoVerify": false, "action": "VIEW", "data": [{ "scheme": "quanux" }], "category": ["BROWSABLE","DEFAULT","LEANBACK_LAUNCHER"] }]
    },
    "platforms": ["android"]
  }
}
EOF

mkdir -p "$TV/assets"
cat >"$TV/App.tsx"<<'EOF'
import React from "react";
import { View, Text } from "react-native";

export default function App() {
  return (
    <View style={{ flex: 1, backgroundColor: "#000", alignItems: "center", justifyContent: "center" }}>
      <Text style={{ color: "#fff", fontSize: 28 }}>QuanuX TV</Text>
      <Text style={{ color: "#aaa" }}>Android TV focused UI goes here.</Text>
    </View>
  );
}
EOF

cat >"$TV/.gitignore"<<'EOF'
node_modules
.expo
dist
.DS_Store
EOF

cat >"$TV/README.md"<<'EOF'
# QuanuX TV (Android TV via Expo/React Native)
- Requires Android TV emulator/device.
- `pnpm i`
- `pnpm android`
EOF

# =====================================================================================
# summary
# =====================================================================================
echo "Scaffold complete under ./client"
find "$CLIENT" -maxdepth 2 -type d -print | sed "s|$ROOT/||"
