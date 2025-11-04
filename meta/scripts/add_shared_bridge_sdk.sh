#!/usr/bin/env bash
set -euo pipefail

# Run from repo root (must see ./server and ./client)
test -d server || { echo "Run from repo root (must see ./server)"; exit 1; }
test -d client || { echo "Missing ./client — run your frontend scaffold first."; exit 1; }

ROOT="$PWD"
SHARED="$ROOT/client/src/lib"
WEB="$ROOT/client/web"
TAURI="$ROOT/client/desktop/tauri-app"

mkdir -p "$SHARED"

# 1) Shared config: resolve/set API base with smart defaults
cat >"$SHARED/config.ts"<<'EOF'
let _manualBase: string | null = null;

function isBrowser(): boolean {
  return typeof window !== "undefined";
}
function isTauri(): boolean {
  // Vite-in-Tauri still has import.meta.env, but we can signal via a sentinel on window.
  return isBrowser() && ("__TAURI_IPC__" in (window as any));
}

export function setApiBase(url: string | null) {
  _manualBase = url || null;
  if (isBrowser() && url) {
    try { localStorage.setItem("QUANUX_API_BASE", url); } catch {}
  }
}

export function getApiBase(): string {
  // 1) manual override (setApiBase)
  if (_manualBase) return _manualBase;

  // 2) global window override (lets you inject at runtime)
  if (isBrowser() && (window as any).QUANUX_API_BASE) {
    return (window as any).QUANUX_API_BASE as string;
  }

  // 3) persisted setting
  if (isBrowser()) {
    try {
      const stored = localStorage.getItem("QUANUX_API_BASE");
      if (stored) return stored;
    } catch {}
  }

  // 4) env (works in Vite + Tauri builds)
  const envBase = (import.meta as any).env?.VITE_API_BASE;
  if (typeof envBase === "string" && envBase.trim().length > 0) return envBase;

  // 5) auto-defaults
  if (isTauri()) return "http://127.0.0.1:8077";
  return "/api";
}
EOF

# 2) Shared bridge helpers
cat >"$SHARED/bridge.ts"<<'EOF'
import { getApiBase } from "./config";

export async function fetchJSON<T=unknown>(path: string, init?: RequestInit): Promise<T> {
  const base = getApiBase();
  const url = path.startsWith("http") ? path : `${base.replace(/\/+$/, "")}/${path.replace(/^\/+/, "")}`;
  const res = await fetch(url, {
    headers: { "accept": "application/json", ...(init?.headers || {}) },
    ...init
  });
  if (!res.ok) {
    const text = await res.text().catch(() => "");
    throw new Error(`HTTP ${res.status} on ${url}${text ? ` — ${text}` : ""}`);
  }
  const ct = res.headers.get("content-type") || "";
  return (ct.includes("application/json") ? res.json() : (res.text() as any)) as Promise<T>;
}

export async function health(): Promise<any> {
  // convention: health lives at /health on the API base
  return fetchJSON("health");
}
EOF

# 3) Patch web Health.tsx to use shared bridge (if present)
if [ -f "$WEB/src/ui/Health.tsx" ]; then
  sed -i '1i import { health } from "../../../src/lib/bridge";' "$WEB/src/ui/Health.tsx" || true
  # Replace the existing fetch/ping implementation with bridge helper
  awk '
    BEGIN{skip=0}
    /export const Health: React.FC = \(\) => {/ {print; print "  const [status, setStatus] = React.useState<null | { ok: boolean; details?: any }>(null);"; print "  const [err, setErr] = React.useState<string | null>(null);"; print ""; print "  const ping = React.useCallback(async () => {"; print "    try {"; print "      setErr(null);"; print "      const json = await health();"; print "      setStatus({ ok: true, details: json });"; print "    } catch (e: any) {"; print "      setStatus({ ok: false });"; print "      setErr(e?.message || \"Request failed\");"; print "    }"; print "  }, []);"; skip=1; next}
    skip && /return \(/ {print "  React.useEffect(() => { void ping(); }, [ping]);"; skip=0}
    skip==0 {print}
  ' "$WEB/src/ui/Health.tsx" >"$WEB/src/ui/Health.tsx.tmp" && mv "$WEB/src/ui/Health.tsx.tmp" "$WEB/src/ui/Health.tsx" || true
fi

# 4) Web .env.example: ensure VITE_API_BASE=/api suggestion exists
if [ -f "$WEB/.env.example" ]; then
  grep -q 'VITE_API_BASE=' "$WEB/.env.example" || cat >>"$WEB/.env.example"<<'EOF'

# Shared API base for web (defaults to /api on Vercel)
VITE_API_BASE=/api
EOF
fi

# 5) Tauri alias: point @quanux/shared/* -> client/src/*
if [ -f "$TAURI/vite.config.ts" ]; then
  cat >"$TAURI/vite.config.ts"<<'EOF'
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: {
      "@quanux/shared": path.resolve(__dirname, "../../src")
    }
  }
});
EOF
fi

if [ -f "$TAURI/tsconfig.json" ]; then
  # Re-write with paths mapping
  cat >"$TAURI/tsconfig.json"<<'EOF'
{
  "compilerOptions": {
    "target": "ES2020",
    "module": "ESNext",
    "jsx": "react-jsx",
    "moduleResolution": "Bundler",
    "strict": true,
    "skipLibCheck": true,
    "noEmit": true,
    "baseUrl": ".",
    "paths": {
      "@quanux/shared/*": ["../../src/*"]
    }
  },
  "include": ["src"]
}
EOF
fi

# 6) Tauri App example: show current base + quick setter UI (non-blocking)
TAURI_APP="$TAURI/src/main.tsx"
if [ -f "$TAURI_APP" ]; then
  cat >"$TAURI_APP"<<'EOF'
import React from "react";
import { createRoot } from "react-dom/client";
import { getApiBase, setApiBase } from "@quanux/shared/lib/config";
import { health } from "@quanux/shared/lib/bridge";

const App = () => {
  const [base, setBase] = React.useState<string>(getApiBase());
  const [msg, setMsg] = React.useState<string>("");

  const probe = async () => {
    try {
      const h = await health();
      setMsg(`Health OK: ${JSON.stringify(h)}`);
    } catch (e: any) {
      setMsg(`Health error: ${e?.message || "failed"}`);
    }
  };

  return (
    <div style={{ padding: 16, fontFamily: "Inter, system-ui, Arial" }}>
      <h1>QuanuX Desktop (Tauri)</h1>
      <p>Current API base: <code>{base}</code></p>
      <div style={{ display: "flex", gap: 8 }}>
        <input
          style={{ padding: 6, minWidth: 380 }}
          value={base}
          onChange={(e) => setBase(e.target.value)}
          placeholder="http://127.0.0.1:8077 or https://your-remote-host"
        />
        <button
          onClick={() => { setApiBase(base); setMsg("Saved new base URL"); }}
          style={{ padding: "6px 10px", cursor: "pointer" }}
        >
          Save
        </button>
        <button onClick={probe} style={{ padding: "6px 10px", cursor: "pointer" }}>Probe health</button>
      </div>
      <div style={{ marginTop: 10 }}>{msg}</div>
    </div>
  );
};

createRoot(document.getElementById("root")!).render(<App />);
EOF
fi

echo "Shared bridge SDK installed."
echo " - client/src/lib/config.ts"
echo " - client/src/lib/bridge.ts"
echo "Web updated:"
echo " - Patched web Health.tsx to use shared fetch"
echo " - Ensured .env.example suggests VITE_API_BASE=/api"
echo "Tauri updated:"
echo " - Added alias @quanux/shared -> client/src"
echo " - Rewrote vite.config.ts + tsconfig.json for alias"
echo " - Replaced desktop App with runtime API base selector"
echo ""
echo "Next:"
echo "  - Web: set VITE_API_BASE=/api (or leave default) and run pnpm dev"
echo "  - Tauri: you can type a remote URL in the UI and Save"
