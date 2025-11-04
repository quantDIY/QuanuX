#!/usr/bin/env bash
set -euo pipefail

# Run from repo root (must see these)
test -d client/desktop/tauri-app || { echo "Missing client/desktop/tauri-app"; exit 1; }
test -d client || { echo "Missing client/"; exit 1; }

ROOT="$PWD"
SHARED="$ROOT/client/src/lib"
TAURI="$ROOT/client/desktop/tauri-app"
TAURI_RS="$TAURI/src-tauri/src"

mkdir -p "$SHARED" "$TAURI/src" "$TAURI_RS"

# 1) Shared profiles: load/save + default Local/Remote + port allocator hint
cat >"$SHARED/profiles.ts"<<'EOF'
export type ServerProfile = {
  id: string;
  name: string;        // "Local (8077)" or "My Remote"
  apiBase: string;     // e.g. http://127.0.0.1:8077  or https://my-host
  preferredPort?: number; // optional hint for local bridge processes
};

const LS_KEY = "QUANUX_SERVER_PROFILES_V1";
const LS_ACTIVE = "QUANUX_ACTIVE_PROFILE_ID";

export function loadProfiles(): ServerProfile[] {
  try {
    const raw = localStorage.getItem(LS_KEY);
    if (raw) return JSON.parse(raw);
  } catch {}
  // defaults
  const def: ServerProfile[] = [
    { id: "local-8077", name: "Local (8077)", apiBase: "http://127.0.0.1:8077", preferredPort: 8077 },
    { id: "remote-sample", name: "Remote (sample)", apiBase: "https://YOUR-REMOTE-URL" }
  ];
  try { localStorage.setItem(LS_KEY, JSON.stringify(def)); } catch {}
  return def;
}

export function saveProfiles(list: ServerProfile[]) {
  try { localStorage.setItem(LS_KEY, JSON.stringify(list)); } catch {}
}

export function getActiveProfileId(): string | null {
  try { return localStorage.getItem(LS_ACTIVE); } catch { return null; }
}
export function setActiveProfileId(id: string | null) {
  try {
    if (id) localStorage.setItem(LS_ACTIVE, id);
    else localStorage.removeItem(LS_ACTIVE);
  } catch {}
}

// find a free port by probing 8077..8099 conceptually; the actual check is via Tauri command
export async function findFreeLocalPort(): Promise<number> {
  for (let p = 8077; p <= 8099; p++) {
    const ok = await window.__TAURI__?.invoke?.("check_port_free", { port: p }).catch(() => false);
    if (ok) return p;
  }
  // fallback
  return 8077;
}

export function cloneProfile(p: ServerProfile, overrides: Partial<ServerProfile>): ServerProfile {
  return { ...p, ...overrides, id: `${p.id}-${Math.random().toString(36).slice(2, 8)}` };
}
EOF

# 2) Tauri React: startup profile chooser + tabbed sessions
cat >"$TAURI/src/main.tsx"<<'EOF'
import React from "react";
import { createRoot } from "react-dom/client";
import { getApiBase, setApiBase } from "@quanux/shared/lib/config";
import { health } from "@quanux/shared/lib/bridge";
import { loadProfiles, saveProfiles, getActiveProfileId, setActiveProfileId, ServerProfile, findFreeLocalPort, cloneProfile } from "@quanux/shared/lib/profiles";

type Session = { id: string; title: string; profile: ServerProfile };

function StartupPicker({ onChoose }: { onChoose: (p: ServerProfile) => void }) {
  const [profiles, setProfiles] = React.useState<ServerProfile[]>(loadProfiles());
  const [name, setName] = React.useState("");
  const [apiBase, setApi] = React.useState("");

  const choose = (p: ServerProfile) => {
    setActiveProfileId(p.id);
    onChoose(p);
  };

  const add = async () => {
    let p: ServerProfile = { id: `p-${Date.now()}`, name: name || apiBase, apiBase: apiBase.trim() };
    if (p.apiBase.startsWith("local://")) {
      // nice sugar: "local://auto" uses a free port
      const free = await findFreeLocalPort();
      p = { ...p, name: `Local (${free})`, apiBase: `http://127.0.0.1:${free}`, preferredPort: free };
    }
    const next = [...profiles, p];
    setProfiles(next); saveProfiles(next);
    choose(p);
  };

  return (
    <div style={{ padding: 16, fontFamily: "Inter, system-ui, Arial" }}>
      <h2>Select a QuanuX server</h2>
      <ul>
        {profiles.map(p => (
          <li key={p.id} style={{ margin: "6px 0" }}>
            <button onClick={() => choose(p)} style={{ padding: "6px 10px", cursor: "pointer" }}>
              {p.name} <small style={{ opacity: 0.7 }}>({p.apiBase})</small>
            </button>
          </li>
        ))}
      </ul>
      <div style={{ marginTop: 12, borderTop: "1px solid #ddd", paddingTop: 12 }}>
        <h3>Add server</h3>
        <div style={{ display: "flex", gap: 8, flexWrap: "wrap" }}>
          <input placeholder="Display name (optional)" value={name} onChange={e => setName(e.target.value)} style={{ padding: 6, minWidth: 260 }} />
          <input placeholder="e.g. https://host OR local://auto" value={apiBase} onChange={e => setApi(e.target.value)} style={{ padding: 6, minWidth: 320 }} />
          <button onClick={add} style={{ padding: "6px 10px", cursor: "pointer" }}>Add</button>
        </div>
      </div>
    </div>
  );
}

function TabBar({ sessions, activeId, onSelect, onClose, onNewFrom }: {
  sessions: Session[]; activeId: string;
  onSelect: (id: string) => void; onClose: (id: string) => void; onNewFrom: (s: Session) => void;
}) {
  return (
    <div style={{ display: "flex", gap: 6, borderBottom: "1px solid #ddd", padding: 6 }}>
      {sessions.map(s => (
        <div key={s.id}
          style={{
            padding: "6px 10px",
            borderRadius: 6,
            background: s.id === activeId ? "#eef3ff" : "#f6f6f6",
            border: "1px solid #ddd",
            display: "flex", alignItems: "center", gap: 8
          }}>
          <button onClick={() => onSelect(s.id)} style={{ all: "unset", cursor: "pointer" }}>
            {s.title}
          </button>
          <button title="Duplicate" onClick={() => onNewFrom(s)} style={{ all: "unset", cursor: "pointer" }}>⧉</button>
          <button title="Close" onClick={() => onClose(s.id)} style={{ all: "unset", cursor: "pointer" }}>✕</button>
        </div>
      ))}
    </div>
  );
}

function SessionPane({ session }: { session: Session }) {
  const [base, setBase] = React.useState<string>(session.profile.apiBase);
  const [msg, setMsg] = React.useState<string>("");

  React.useEffect(() => {
    setApiBase(base);
  }, [base]);

  const probe = async () => {
    try {
      const h = await health();
      setMsg(`Health OK: ${JSON.stringify(h)}`);
    } catch (e: any) {
      setMsg(`Health error: ${e?.message || "failed"}`);
    }
  };

  return (
    <div style={{ padding: 12 }}>
      <div style={{ display: "flex", gap: 8, alignItems: "center", flexWrap: "wrap" }}>
        <label>API base:</label>
        <input value={base} onChange={e => setBase(e.target.value)} style={{ padding: 6, minWidth: 420 }} />
        <button onClick={probe} style={{ padding: "6px 10px", cursor: "pointer" }}>Probe</button>
      </div>
      <div style={{ marginTop: 8, fontFamily: "ui-monospace, SFMono-Regular, Menlo, monospace" }}>{msg}</div>
    </div>
  );
}

const App = () => {
  const [sessions, setSessions] = React.useState<Session[]>([]);
  const [active, setActive] = React.useState<string>("");

  const addSessionFor = (p: ServerProfile) => {
    const s: Session = { id: `s-${Date.now()}-${Math.random().toString(36).slice(2,6)}`, title: p.name, profile: p };
    setSessions(v => [...v, s]); setActive(s.id);
    setApiBase(p.apiBase);
  };

  const duplicateSession = (s: Session) => {
    const np = cloneProfile(s.profile, {}); // keep same apiBase; users can edit
    addSessionFor(np);
  };

  React.useEffect(() => {
    const activeId = getActiveProfileId();
    const profs = loadProfiles();
    const chosen = profs.find(p => p.id === activeId);
    if (!chosen) return; // show picker
    addSessionFor(chosen);
  }, []);

  if (sessions.length === 0) {
    return <StartupPicker onChoose={addSessionFor} />;
  }

  const current = sessions.find(s => s.id === active) || sessions[0];

  return (
    <div style={{ fontFamily: "Inter, system-ui, Arial" }}>
      <TabBar
        sessions={sessions}
        activeId={current.id}
        onSelect={setActive}
        onClose={(id) => {
          setSessions(v => v.filter(s => s.id !== id));
          if (active === id && sessions.length > 1) {
            const next = sessions.find(s => s.id !== id);
            if (next) setActive(next.id);
          }
        }}
        onNewFrom={duplicateSession}
      />
      <SessionPane session={current} />
      <div style={{ padding: 6, borderTop: "1px solid #eee", display: "flex", gap: 8 }}>
        <button
          onClick={() => {
            const p = loadProfiles()[0];
            if (p) addSessionFor(p);
          }}
          style={{ padding: "6px 10px", cursor: "pointer" }}
        >
          + New Tab (from first profile)
        </button>
        <button
          onClick={() => (window as any).__TAURI__?.invoke?.("new_window", { label: `w-${Date.now()}`, title: "QuanuX Window" })}
          style={{ padding: "6px 10px", cursor: "pointer" }}
        >
          + New Window
        </button>
      </div>
    </div>
  );
};

createRoot(document.getElementById("root")!).render(<App />);
EOF

# 3) Tauri Rust: add commands for port check and new window
cat >"$TAURI_RS/main.rs"<<'EOF'
#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use tauri::{Manager, WindowBuilder, WindowUrl};
use std::net::{TcpListener, SocketAddr};

#[tauri::command]
fn check_port_free(port: u16) -> bool {
  let addr = SocketAddr::from(([127, 0, 0, 1], port));
  TcpListener::bind(addr).is_ok()
}

#[tauri::command]
fn new_window(app: tauri::AppHandle, label: String, title: String) -> Result<(), String> {
  WindowBuilder::new(&app, label, WindowUrl::App("index.html".into()))
    .title(title)
    .build()
    .map(|_| ())
    .map_err(|e| e.to_string())
}

fn main() {
  tauri::Builder::default()
    .invoke_handler(tauri::generate_handler![check_port_free, new_window])
    .run(tauri::generate_context!())
    .expect("error while running QuanuX desktop");
}
EOF

# 4) Ensure the shared alias already exists (from prior step); if not, add it
if ! grep -q "@quanux/shared" "$TAURI/vite.config.ts" 2>/dev/null; then
  cat >"$TAURI/vite.config.ts"<<'EOF'
import { defineConfig } from "vite";
import react from "@vitejs/plugin-react";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));

export default defineConfig({
  plugins: [react()],
  resolve: {
    alias: { "@quanux/shared": path.resolve(__dirname, "../../src") }
  }
});
EOF
fi

echo "Tauri multi-instance + server picker installed."
echo "Files updated:"
echo " - client/src/lib/profiles.ts"
echo " - client/desktop/tauri-app/src/main.tsx"
echo " - client/desktop/tauri-app/src-tauri/src/main.rs"
echo "You can open multiple tabs, new windows, or run multiple app processes."
